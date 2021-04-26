# Makefile
#
# Oleksandra Baga
# Milos Budimir
# Jannes Volkens
# Michael Zent
#
# Telematik, Prof. M. Wählisch
# WS 2020/21 FU Berlin

AUTH = authns
RRES = recres
STUB = stub

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wno-deprecated -O2
LIBS = -ljsoncpp

all: $(AUTH) $(RRES) $(STUB)

$(AUTH):
	$(CXX) $(CXXFLAGS) src/authns.cpp $(LIBS) -o $@

$(RRES):
	$(CXX) $(CXXFLAGS) src/recres.cpp $(LIBS) -o $@

$(STUB):
	$(CXX) $(CXXFLAGS) src/stub.cpp $(LIBS) -o $@

clean:
	rm $(AUTH) $(RRES) $(STUB)
