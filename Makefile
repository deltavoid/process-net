

compile : *.h *.cpp
	g++ -std=c++11 -c *.cpp #-lpthread -lpcap

main : *.h *.cpp
	g++ -std=c++11 -o main *.cpp -lpthread -lpcap