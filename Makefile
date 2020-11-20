prog: src/authorative_server.c src/stub_resolver.c 
	gcc -o server src/authorative_server.c
	gcc -o client src/stub_resolver.c