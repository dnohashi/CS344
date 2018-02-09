/*
 * Dean Ohashi
 * CS344
 * Program 2 - adventure
 * ohashid.buildrooms.c
 *
 * Note: Program written using CS344 Lecture Note 2.2 Program Outlining in Program 2 template
 *       Alterations made for functions that initially returned bool to return ints 0/1 to represent
 *       boolean values
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#define MAX_ROOMS 6


char* roomNames[] = {"Nursery", "BedOne", "BedTwo", "Living", "Kitchen", "Basement",
			 "Attic", "Garage", "Bathroom", "Office"};

/*
 Struct to hold Room variables.
*/
struct Room{
	char *roomName;
	char *roomType;
	int numConnections;
	struct Room* connections[MAX_ROOMS];
};

struct Room rooms[7];

int IsGraphFull();
void AddRandomConnection();
struct Room GetRandomRoom();
int CanAddConnectionFrom(struct Room);
int ConnectionAlreadyExists(struct Room x, struct Room y);
void ConnectRoom(struct Room x, struct Room y);
int IsSameRoom(struct Room x, struct Room y);
int roomUsed(char *string);

/*
 Tracks rooms used so far, returns 1 if room has been used, else return 0
*/
int roomUsed(char *string){
	int i;
	for(i = 0; i < 7; i++){
		if(rooms[i].roomName == string) return 1;
	}

	return 0;
}

/*
 Checks if graph is full. Returns 1 if not full, else returns 0.
*/
int IsGraphFull(){
	int i;
	int bool = 1;

	for(i = 0; i < 7; i++){
		if(rooms[i].numConnections < 3) bool = 0;
	}

	return bool;
}

/*
 Gets two rooms, validates room, verifies they can get connected, and connects the two rooms.
*/
void AddRandomConnection(){
	struct Room A;
	struct Room B;
	while(1){
		A = GetRandomRoom();
		if(CanAddConnectionFrom(A) == 1){
			printf("Found A: %s\n", A.roomName);
			break;
		}
	}

	do{
		B = GetRandomRoom();
		printf("in the do while loop\n");
		printf("B: %s\n", B.roomName);
	}while(CanAddConnectionFrom(B) == 0 || IsSameRoom(A, B) == 1 || ConnectionAlreadyExists(A, B) == 1);

	printf("out of the do while loop\n");

	printf("A: %s\n", A.roomName);
	printf("B: %s\n", B.roomName);
	ConnectRoom(A, B);
	ConnectRoom(B, A);
}

/*
 Returns random room.
*/
struct Room GetRandomRoom(){
	int roomNum = rand() % 7;
	return rooms[roomNum];
}

/*
 Checks to see if connections can be made from given room.
 Returns 1 if possible, returns 0 if not
*/
int CanAddConnectionFrom(struct Room x){
	int bool = 1;
	if(x.numConnections == 6) bool = 0;
	return bool;
}

/*
 Checks if connection between two rooms already exists.
 Returns 1 if connection exists, else returns 0.
*/
int ConnectionAlreadyExists(struct Room x, struct Room y){
	int i;
	int bool = 0;
	printf("test x name: %s\n", x.roomName);
	for(i = 0; i < x.numConnections; i++){
		printf("X connection: %s\n", x.connections[i]->roomName);
		if(x.connections[i]->roomName ==  y.roomName) bool = 1;
	}
	

	printf("returning connectexists: %d\n\n", bool);	

	return bool;
}

/*
 Function to connect two rooms when called on by AddRandomConnections()
*/
void ConnectRoom(struct Room x, struct Room y){
	int i, j, k;
	for(i = 0; i < 7; i++){
		if(rooms[i].roomName == x.roomName){
			for(j = 0; j < 6; j++){
				if(rooms[i].connections[j] == NULL){
					rooms[i].connections[j] = &y;
					rooms[i].numConnections++;
					rooms[i].connections[j]->numConnections++;
					k = i;
					break;
				}
			}
		}
	}
}

/*
 Checks to see if two rooms are the same
 Return 1 if rooms are the same, else return 0 
 */
int IsSameRoom(struct Room x, struct Room y){
	int bool = 0;
	if(x.roomName == y.roomName) bool = 1;
	
	return bool;
}

int main(){
	srand(time(NULL));

	int i, index;
	int bool = 1;
	for(i = 0; i < 7; i++){
		while(bool){
			index = rand() % 7;
			if(roomUsed(roomNames[index])==0) bool = 0;
		}

		rooms[i].roomName = roomNames[index];
		rooms[i].numConnections = 0;

		if(i == 0) rooms[i].roomType = "START_ROOM";
		else if(i == 6) rooms[i].roomType = "END_ROOM";
		else rooms[i].roomType = "MID_ROOM";
		
		bool = 1;
	}
	int j;
	for(j = 0; j < 7; j++){
		printf("Room: %d | Name: %s | Type: %s\n\n", j + 1, rooms[j].roomName, rooms[j].roomType);	
	}	
	
		
	while(IsGraphFull() == 0){
		printf("adding connections\n\n");
		AddRandomConnection();

		printf("iterating again\n\n");
	}
	return 0;
}
