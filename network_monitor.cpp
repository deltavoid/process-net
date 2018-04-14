
#include "network_monitor.h"



void NetworkMonitor::dispatch() {
		char errbuff[PCAP_ERRBUF_SIZE];
		//char *dev = pcap_lookupdev(errbuff);
		const char* dev = "enp179s0f1";
		
		this->handle = pcap_open_live(dev, BUFSIZ, 0, 1000, errbuff);

		this->linkType = pcap_datalink(this->handle);

		int err = pthread_create(&this->ptid, NULL, loop, this);

	}

void NetworkMonitor::addProcess(int pid) {
	pthread_mutex_lock(&pmutex);
		size_t size = this->processs.size();
		for (int i = 0; i< size; i++) {
			Process *now = this->processs[i];
			if (now->pid == pid) {
				return;
			}
		}
		this->processs.push_back(new Process(pid));

	for (int i = 0; i < this->processs.size(); i++)
	    std::cout << this->processs[i]->pid << std::endl;

	pthread_mutex_unlock(&pmutex);
}

void NetworkMonitor::removeProcess(int pid) {
	pthread_mutex_lock(&pmutex);
		std::vector<Process*>::iterator it = this->processs.begin();
		while (it != this->processs.end()) {
			Process *pro = *it;
			if (pro->pid == pid) {
				it = this->processs.erase(it);
				delete pro;
				break;
			} else {
				it++;
			}
		}
	pthread_mutex_unlock(&pmutex);
}

double NetworkMonitor::getProcessBandwidth(int pid)
{
	pthread_mutex_lock(&pmutex);
	double out = -1;
	size_t size = this->processs.size();
    for (int i = 0; i< size; i++) {
        Process *now = this->processs[i];
        if (now->pid == pid) {
            out = now->sudu;
            break;
        }
    }
	pthread_mutex_unlock(&pmutex);
	return out;
}


void* NetworkMonitor::wakeUp(void *arg){
	pthread_detach(pthread_self());
	NetworkMonitor *app = (NetworkMonitor*)arg;
	sleep(app->time);
	pcap_breakloop(app->handle);
	return ((void*)0);
}


void* NetworkMonitor::loop(void *arg){
	NetworkMonitor *app = (NetworkMonitor*)arg;
	timeval begin, end;
	pthread_t ptid;
	while (true) {
		gettimeofday(&begin, NULL);



		int err = pthread_create(&ptid, NULL, wakeUp, app);

		pcap_loop(app->handle, -1, processCallBack, (u_char*)app);
		size_t size = app->processs.size();
		
		gettimeofday(&end, NULL);

		double sec = end.tv_sec - begin.tv_sec;
		sec += ((double)(end.tv_usec - begin.tv_usec)) / 1000000;


		pthread_mutex_lock(&app->pmutex);
		for (int i = 0; i < size; i++) {
			Process *now = app->processs[i];
			double all = (double)now->len;
			now->len = 0;
			double su = all / sec / 1000;
			now->sudu = su;
			now->refershInodes();
			if (now->inodes == NULL) {
				app->res.push_back(now->pid);
			}
		}
		pthread_mutex_unlock(&app->pmutex);


		for (int i = 0; i < app->res.size(); i++) {
			int pid = app->res[i];
			app->removeProcess(pid);
		}
		app->res.clear();
		app->refreshConnection();
	}
}


void NetworkMonitor::processCallBack(u_char *userData, const  pcap_pkthdr *header, const u_char *packet){
	NetworkMonitor *app = (NetworkMonitor*)userData;

	switch (app->linkType) {
		case (DLT_EN10MB):
			app->dp_parse_ethernet (header, packet);
			break;
			/*
		case (DLT_PPP):
			dp_parse_ppp (handle, header, packet);
			break;
		case (DLT_LINUX_SLL):
			dp_parse_linux_cooked (handle, header, packet);
			break;
		case (DLT_RAW):
		case (DLT_NULL):
			// hope for the best
			dp_parse_ip (handle, header, packet);
			break;)*/
		default:
			printf("unknown link type");
			break;
	}
}


void NetworkMonitor::dp_parse_ethernet (const pcap_pkthdr * header, const u_char * packet){
		const struct ether_header * ethernet = (struct ether_header *)packet;
		u_char * payload = (u_char *) packet + sizeof (struct ether_header);


		/* parse payload */
		switch (ethernet->ether_type)
		{
			case (0x0008):
				this->dp_parse_ip (header, payload);
				break;
			case (0xDD86):
				//dp_parse_ip6 (handle, header, payload);
				break;
			default:
				// TODO: maybe support for other protocols apart from IPv4 and IPv6 
				break;
		}
}

void NetworkMonitor::dp_parse_ip (const pcap_pkthdr * header, const u_char * packet){
		const struct ip * ip = (struct ip *) packet;

		this->info.sa_family = AF_INET;
		this->info.ip_src = ip->ip_src;
		this->info.ip_dst = ip->ip_dst;

		u_char * payload = (u_char *) packet + sizeof (struct ip);
		switch (ip->ip_p)
		{
			case (6):
				this->dp_parse_tcp (header, payload);
				break;
			default:
				// TODO: maybe support for non-tcp IP packets
				break;
		}
}

void NetworkMonitor::dp_parse_tcp (const pcap_pkthdr * header, const u_char * packet){
		struct tcphdr * tcp = (struct tcphdr *) packet;

		unsigned long inode = this->con->getConnectionInode(this->info.ip_src, ntohs(tcp->source), this->info.ip_dst, ntohs(tcp->dest));
		//std::cout << "inode: " << inode << std::endl;
		pthread_mutex_lock(&pmutex);
		size_t size = this->processs.size();
		for (int i = 0; i< size; i++) {
			Process *now = this->processs[i];

			if (now->hasInode(inode)) {
				
				now->len += header->len;
                //std::cout << "pid: " << now->pid << std::endl;
				//std::cout << "inode: " << inode << std::endl;
				//std::cout << "len: " << now->len << std::endl;
				break;
			}
		}
		pthread_mutex_unlock(&pmutex);
		/*
		if (this->process->hasInode(inode)) {
			this->len += header->len;
		}*/
}





