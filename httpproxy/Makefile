proxyserver: proxyserver.o log.o
	g++ -pthread -pedantic -o proxyserver proxyserver.o log.o
proxyserver.o: proxyserver.cpp proxyserver.h
	g++ -std=gnu++11 -O3 -pthread -pedantic -Wall -Werror -c proxyserver.cpp
log.o: log.cpp log.h
	g++ -std=gnu++11 -O3 -pthread -pedantic -Wall -Werror -c log.cpp
clean:
	rm -rf proxyserver 
