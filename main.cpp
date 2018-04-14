#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>
#include "network_monitor.h"

int pid;
NetworkMonitor* network_monitor;

int main(int argc, char** argv)
{
    assert(argc > 1);
    sscanf(argv[1], "%d", &pid);
    printf("pid: %d\n", pid);

    network_monitor = new NetworkMonitor("ens33", 1);
    network_monitor->addProcess(pid);

    for (int i = 0; i < 10; i++)
    {
        printf("%lf\n", network_monitor->getProcessBandwidth(pid));
        sleep(1);
    }

    return 0;

}