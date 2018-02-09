all: ohashid.buildrooms.c
	gcc -o buildrooms ohashid.buildrooms.c

clean:
	rm buildrooms
