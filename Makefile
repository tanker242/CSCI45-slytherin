a.out: main.c
	gcc main.c -lwiringPi -std=c11 
#-Og
clean:
	rm a.out
