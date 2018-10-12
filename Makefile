
test: test.o token.o command.o
	gcc test.o token.o command.o -o test
command.o: command.h command.c
	gcc -c command.c
test.o: test.c token.h
	gcc -c test.c
token.o: token.c token.h
	gcc -c token.c
clean:
	rm *.o
