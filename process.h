#ifndef PROCESS_H
#define PROCESS_H
#include <string>
#include <vector>



class Process
{public:
    int pid;
    std::vector<int> inodes;
    //std::vector<std::string> sockets;
    double speed;

    Process(int pid);

    void update_inodes();
    
};

#endif