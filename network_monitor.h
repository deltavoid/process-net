#ifndef __NETWORK_MONITOR_H
#define __NETWORK_MONITOR_H
#include <iostream>
#include <map>
#include <set>
#include <vector>



#include <pthread.h>

#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <iostream>

#include <string.h>
#include <string>

#include <asm/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pwd.h>
#include <map>

#include <netinet/in.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>


#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <cstdlib>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>
#include <time.h>

#include <pcap.h>

#include "conninode.h"
#include "process.h"



class NetworkMonitor{
public:
	NetworkMonitor(const char* dev, int time) {
        this->device = dev;
        this->time = time;
		this->con = new Connection();
		pthread_mutex_init(&pmutex, NULL);

		dispatch(); //init
	}

	~NetworkMonitor() {
		size_t size = this->processs.size();
		for (int i = 0; i< size; i++) {
			Process *now = this->processs[i];
			delete now;
		}
		delete this->con;
	}

	void refreshConnection() {
		delete this->con;
		this->con = new Connection();
	}


	void dispatch();

	static void* calc_speed(void *arg);

	static void* fetch_package(void *arg);

	static void processCallBack(u_char *userData, const  pcap_pkthdr *header, const u_char *packet);


	void addProcess(int pid);

	void removeProcess(int pid);

	double getProcessBandwidth(int pid);

	void dp_parse_ethernet (const pcap_pkthdr * header, const u_char * packet);

	void dp_parse_ip (const pcap_pkthdr * header, const u_char * packet);

	void dp_parse_tcp (const pcap_pkthdr * header, const u_char * packet);

	int time;
	std::string device;
	pcap_t *handle;
	int linkType;
	dpargs info;
	pthread_t fetch_package_thread, calc_speed_thread;
	long pid;
	std::vector<Process*> processs;
	std::vector<int> res;
	pthread_mutex_t pmutex;
	
	Connection *con;
	
	
	
};






#endif