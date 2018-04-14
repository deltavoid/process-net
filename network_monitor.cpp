#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "network_monitor.h"
using namespace std;

void NetworkMonitor::add_process(int pid)
{
}

void NetworkMonitor::remove_process(int pid)
{
}

double NetworkMonitor::get_process_speed(int pid)
{
}

NetworkMonitor::NetworkMonitor()
{
}

NetworkMonitor::~NetworkMonitor()
{
    pcap_breakloop(handle);
}

void* NetworkMonitor::fetch_package_entry(void* arg)
{
    char errbuff[PCAP_ERRBUF_SIZE];
	char *dev = pcap_lookupdev(errbuff);
	
    handle = pcap_open_live(dev, BUFSIZ, 0, 1000, errbuff);
    linkType = pcap_datalink(this->handle);

    pcap_loop(handle, -1, processCallBack, NULL);

    return NULL;
}

void* NetworkMonitor::calc_speed_entry(void* arg)
{
}

void NetworkMonitor::update_processes()
{
}

void NetworkMonitor::update_connections()
{
}

void NetworkMonitor::calc_process_speed(Process* process)
{
}

void NetworkMonitor::processCallBack(u_char *userData, const  pcap_pkthdr *header, const u_char *packet)
{
    switch (linkType) {
		case (DLT_EN10MB):
			app->dp_parse_ethernet (header, packet);
			break;
			/*
		case (DLT_PPP):
			dp_parse_ppp (handle, header, packet);5++
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

void NetworkMonitor::dp_parse_ethernet (const pcap_pkthdr * header, const u_char * packet)
{
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

void NetworkMonitor::dp_parse_ip (const pcap_pkthdr * header, const u_char * packet)
{
}

void NetworkMonitor::dp_parse_tcp (const pcap_pkthdr * header, const u_char * packet)
{
}



