#include "process.h"



Process::Process(int pid) {
		this->pid = pid;
		this->inodes = getProcessSocketInode(this->pid);
}

Process::~Process() {
		delete this->inodes;
}


bool Process::hasInode(long inode) {  //interface
		std::set<long>::iterator it;
		it = this->inodes->find(inode);
		if (it == this->inodes->end()) {
			return false;;
		} else {
			return true;
		}
}

void Process::refershInodes() {  //interface
		if (this->inodes != NULL) {
			std::set<long>::iterator it = this->inodes->begin();
			while (it != this->inodes->end()) {
				this->inodes->erase(it);
				it++;
			}
			this->inodes->clear();
			delete this->inodes;
			this->inodes = NULL;
		}
		this->inodes = getProcessSocketInode(this->pid);
}



bool Process::is_number (char * string) {
	while (*string) {
		if (!isdigit (*string))
			return false;
		string++;
	}
	return true;
}

unsigned long Process::str2ulong (char * ptr) {
	unsigned long retval = 0;

	while ((*ptr >= '0') && (*ptr <= '9')) {
		retval *= 10;
		retval += *ptr - '0';
		ptr++;
	}
	return retval;
}

int Process::str2int (char * ptr) {
	int retval = 0;

	while ((*ptr >= '0') && (*ptr <= '9')) {
		retval *= 10;
		retval += *ptr - '0';
		ptr++;
	}
	return retval;
}


std::set<long>* Process::getProcessSocketInode(long pid) {
	std::set<long>* v = new std::set<long>();
	char temp[50];
	memset(temp, 0, 50);
	sprintf(temp, "%ld", pid);
	size_t dirlen = 10 + strlen(temp);
	char * dirname = (char *) malloc (dirlen * sizeof(char));
	snprintf(dirname, dirlen, "/proc/%s/fd", temp);

	int isExist = access(dirname, F_OK);
	if (isExist == -1) {
		free(dirname);
		printf("process do not exist, pid is : %d\n", (int)pid);
		return NULL;
	}

	DIR * dir = opendir (dirname);

	dirent * entry;

	while ((entry = readdir(dir))) {
		if (entry->d_type != DT_LNK)
			continue;
		//std::cout << "Looking at: " << entry->d_name << std::endl;

		int fromlen = dirlen + strlen(entry->d_name) + 1;
		char * fromname = (char *) malloc (fromlen * sizeof(char));
		snprintf (fromname, fromlen, "%s/%s", dirname, entry->d_name);

		//std::cout << "Linking from: " << fromname << std::endl;

		int linklen = 80;
		char linkname [linklen];
		int usedlen = readlink(fromname, linkname, linklen-1);
		if (usedlen == -1)
		{
			free (fromname);
			continue;
		}
		
		linkname[usedlen] = '\0';
		//std::cout << "Linking to: " << linkname << std::endl;
		//get_info_by_linkname (pid, linkname);
		if (strncmp(linkname, "socket:[", 8) == 0) {
			
			char * ptr = linkname + 8;
			unsigned long inode = str2ulong(ptr);

			//char * progname = getprogname (temp);
			//free(progname);

			v->insert(inode);
		} else {
			//std::cout << "Linkname looked like: " << linkname << endl;
		}
		free (fromname);
	}
	closedir(dir);
	free(dirname);
	return v;

}
