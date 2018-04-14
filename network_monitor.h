#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H
#include <string>
#include <map>
#include <pthread.h>
#include <pcap.h>
#include "process.h"


class NetworkMonitor
{public:
    std::map<int, Process*> processes;
    std::map<int, std::string> connections;
    std::map<std::string, double> flows;
    pthread_t fetch_package_thread, calc_speed_thread;
    pthread_mutex_t mutex_processes, mutex_flows;

    pcap_t* handle;
    int linkType;


    void add_process(int pid);
    void remove_process(int pid);
    double get_process_speed(int pid);

    NetworkMonitor();
    ~NetworkMonitor();

    void* fetch_package_entry(void* arg);
    void* calc_speed_entry(void* arg);

    void update_processes();
    void update_connections();
    void calc_process_speed(Process* process);

    void processCallBack(u_char *userData, const  pcap_pkthdr *header, const u_char *packet);
    void dp_parse_ethernet (const pcap_pkthdr * header, const u_char * packet);
    void dp_parse_ip (const pcap_pkthdr * header, const u_char * packet);
    void dp_parse_tcp (const pcap_pkthdr * header, const u_char * packet);

    static std::string socket2string(int src_ip, int src_port, int dst_ip, int dst_port);
    
};


#endif