
ser6: stream.o ser6.o cli6
	gcc stream.o ser6.o -o ser6
cli6: stream.o cli6.o
	gcc -o cli6 stream.o cli6.o
stream.o: stream.c
	gcc -c stream.c
cli6.o: cli6.c
	gcc -c cli6.c
ser6.o: ser6.c
	gcc -c ser6.c
clean:
	rm *.o
