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
#include <unistd.h>
#include <limits.h>

#define MAX_ROOMS 6

/*
 Declared global array to hold 10 possible room names to be randomly selected
*/
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

/*
 Declared globay array to hold seven rooms
*/
struct Room rooms[7];

char directoryName[NAME_MAX+1];

int IsGraphFull();
void AddRandomConnection();
struct Room* GetRandomRoom();
int CanAddConnectionFrom(struct Room*);
int ConnectionAlreadyExists(struct Room* x, struct Room* y);
void ConnectRoom(struct Room* x, struct Room* y);
int IsSameRoom(struct Room* x, struct Room* y);
int roomUsed(char *string);
void setRooms();
void addRooms();
void addToFile();

/*
 Tracks rooms used so far, returns 1 if room has been used, else return 0
*/
int roomUsed(char *string){
	int i;
	
	//Iterate through all 7 rooms and compare their names to the argument string
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
	//iterate through all 7 rooms to check if their connections are less than 3
	//if connections are less than 3, that given room can still be connected
	for(i = 0; i < 7; i++){
		if(rooms[i].numConnections < 3) bool = 0;
	}

	return bool;
}

/*
 Gets two rooms, validates room, verifies they can get connected, and connects the two rooms.
*/
void AddRandomConnection(){
	//pointers to struct Rooms to get pointers to various rooms[]
	struct Room* A;
	struct Room* B;
	//following while-loop to retrieve first room A, will continue to run until break statement
	while(1){
		A = GetRandomRoom(); //retrieves random room
		if(CanAddConnectionFrom(A) == 1){
			break; //if an appropriate room is found, the loop is exited via break
		}
	}

	do{
		B = GetRandomRoom(); //similar to A, gets random room for struct Room* B
	}while(CanAddConnectionFrom(B) == 0 || IsSameRoom(A, B) == 1 || ConnectionAlreadyExists(A, B) == 1);
	//do-while continues to loop while the retrieved room cannot have additional connections
	//is the same room as A, or already has a pre-existing connection with A

	//calls ConnectRooms() twice to link A--> B and B-->A
	ConnectRoom(A, B); 
	ConnectRoom(B, A);
}

/*
 Returns random room.
*/
struct Room* GetRandomRoom(){
	int roomNum = rand() % 7; //generates random integer between 0 ~ 6 (inclusive)
	return &rooms[roomNum];
}

/*
 Checks to see if connections can be made from given room.
 Returns 1 if possible, returns 0 if not
*/
int CanAddConnectionFrom(struct Room* x){
	int bool = 1; //bool initially set to 1 
	if(x->numConnections == 6) bool = 0; //if the provided room has 6 connections, it is maxed out
	return bool;
}

/*
 Checks if connection between two rooms already exists.
 Returns 1 if connection exists, else returns 0.
*/
int ConnectionAlreadyExists(struct Room* x, struct Room* y){
	int i;
	int bool = 0;
	//will iterate for the Room X's # of connections
	for(i = 0; i < x->numConnections; i++){
		//if one of the connections has the same name as Room Y, a connection exists, bool = 1
		if(x->connections[i]->roomName ==  y->roomName) bool = 1; 
	}
	
	return bool;
}

/*
 Function to connect two rooms when called on by AddRandomConnections()
*/
void ConnectRoom(struct Room* x, struct Room* y){
	int i, j;
	//iterate up to seven times to find the room with the argument room name
	for(i = 0; i < 7; i++){
		if(rooms[i].roomName == x->roomName){
		//once argument name is found, will iterate up to 6 times to find an empty slot
			for(j = 0; j < 6; j++){
				//room connection will be made in empty slot
				if(rooms[i].connections[j] == NULL){
					rooms[i].connections[j] = y; //slot set to room Y
					rooms[i].numConnections++; //connection incremented
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
int IsSameRoom(struct Room* x, struct Room* y){
	int bool = 0;
	//identifies if X and Y have the same room name by comparing string values
	if(x->roomName == y->roomName) bool = 1;
	
	return bool;
}

/*
 Void function to set initial 6 rooms (fill struct Room array)
*/
void setRooms(){
	int i, index;
	int bool = 1;
	//iterate room selections for up to 7 rooms
	for(i = 0; i < 7; i++){
		while(bool){
			index = rand() % 10; //randomize an index value between 0 ~ 9
			if(roomUsed(roomNames[index])==0) bool = 0; //verifies if it has been used
		}
		
		rooms[i].roomName = roomNames[index]; //set room name to randomly selected room
		rooms[i].numConnections = 0; //initialize numConnections to 0

		if(i == 0) rooms[i].roomType = "START_ROOM"; //set beginning room to START_ROOM
		else if(i == 6) rooms[i].roomType = "END_ROOM"; //set final room to END_ROOM
		else rooms[i].roomType = "MID_ROOM"; //all other rooms set to MID_ROOM
		
		bool = 1;
	}
}

/*
 Void function to continue to add rooms while the graph is not full
 Calls on IsGraphFull() and AddRandomConnection()
*/
void addRooms(){
	//continue to add rooms while graph is not full
	while(IsGraphFull() == 0){
		AddRandomConnection();
	}
}

/*
 Void function to change directory to the directory created by makeDirectory()
 and create a file for each room to append all room information to
*/
void addToFile(){
	chdir(directoryName); //change directory to directory made in makeDirectory
	int i, j;
	for(i = 0; i < 7; i++){ //iterate through each room
		FILE * newFile = fopen(rooms[i].roomName, "w"); //create new file with room name, write only
		fprintf(newFile, "ROOM NAME: %s\n", rooms[i].roomName); //print room name into file
		
		for(j = 0; j < rooms[i].numConnections; j++){ //iterate through all the connections
			//print connection number and name to the file
			fprintf(newFile, "CONNECTION %d: %s\n", j + 1, rooms[i].connections[j]->roomName);
		}
		//print room type to file
		fprintf(newFile, "ROOM TYPE: %s\n", rooms[i].roomType);
		fclose(newFile); //close file
	}
}

/*
 Void function to make directory with following format: ohashid.rooms.(pid)
*/
void makeDirectory(){
	char *prefix = "ohashid.rooms."; //prefix for directory to be made
	int pid = getpid(); //gets process id
	snprintf(directoryName, NAME_MAX + 1,"%s%d", prefix, pid);
	mkdir(directoryName, 0777);		
}

int main(){
	srand(time(NULL)); //set seed
	setRooms();
	addRooms();
	makeDirectory();
	addToFile();
	
	return 0;
}
