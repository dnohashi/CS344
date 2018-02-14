all: ohashid.adventure.c
	gcc -o adventure ohashid.adventure.c -lpthread

clean:
	rm buildrooms
