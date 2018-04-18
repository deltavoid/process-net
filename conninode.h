#include "process.h"


#ifndef __CONNINODE_H
#define __CONNINODE_H







class Connection{
public:
	Connection();
	~Connection();
	long getConnectionInode(in_addr ip_src, unsigned short port_src, in_addr ip_dst, unsigned short port_dst);
    void refreshConnectionInode();

    static void addToConnInode (char * buffer, std::map <std::string, unsigned long>* conninode);
    static int addProcInfo (const char * filename, std::map <std::string, unsigned long>* conninode);
	static std::map <std::string, unsigned long>* getRefreshConnInode ();

	
	std::map <std::string, unsigned long> *conninode;
	static const int HASHKEYSIZE = 92;
};



#endif
