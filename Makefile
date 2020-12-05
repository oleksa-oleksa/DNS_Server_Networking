# prog: src/authorative_server.c src/stub_resolver.c 
#	gcc -o server src/authorative_server.c -l json-c
#	gcc -o client src/stub_resolver.c -l json-c

AUTH = server
STUB = client
RESO = resolver

CXX = g++
CXXFLAGS = -Wall -O2 -std=c++11
LIBS = -ljson-c

all: $(AUTH) $(STUB) $(DB) $(RESO)

$(AUTH):
	$(CXX) $(CXXFLAGS) src/dns_db.cpp src/authorative_server.cpp $(LIBS) -o $@

$(DB):
	$(CXX) $(CXXFLAGS) src/dns_db.cpp $(LIBS) -o $@

$(STUB):
	$(CXX) $(CXXFLAGS) src/stub_resolver.cpp $(LIBS) -o $@

$(RESO):
	$(CXX) $(CXXFLAGS) src/dns_db.cpp src/recursive_resolver.cpp $(LIBS) -o $@

clean:
	rm $(AUTH) $(STUB) $(DB) $(RESO)
