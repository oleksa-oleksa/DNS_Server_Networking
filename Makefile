prog: authorative_server.o stub_resolver.o
	gcc -o server authorative_server.o
	gcc -o client stub_resolver.o

server_client.o: authorative_server.c stub_resolver.c
	gcc -c authorative_server.c
	gcc -c stub_resolver.c