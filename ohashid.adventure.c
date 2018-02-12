#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_CONNECTIONS 6
#define MAX_ROOMS 7

struct Room{
	char roomName[25];
	char roomType[25];
	int numConnections;
	char* connected[MAX_CONNECTIONS];
};

struct Room rooms[MAX_ROOMS];

char* roomNames[] = {"Nursery", "BedOne", "BedTwo", "Living", "Kitchen", "Basement",
			"Attic", "Garage", "Bathroom", "Office"};



int start_room, end_room, connect;

void readFiles();
void playGame();
void sendToRead();
void readPerFile(char*string, int);
struct Room* getRoom(char*);


struct Room* getRoom(char roomName[]){
	int i;
	for(i = 0; i < MAX_ROOMS; i++){
		if(strcmp(rooms[i].roomName, roomName)==0){
			return &rooms[i];
		}
	}	
}

void sendToRead(){
	int i;
	int index = 0;
	for(i = 0; i < 10; i++){
		if(access(roomNames[i], F_OK) != -1){
			readPerFile(roomNames[i], index);
			index++;
		}
	}
}

void readPerFile(char* roomName, int index){
	int i;
	FILE *file;
	char buffer[30];
	char* newLine;

	file = fopen(roomName, "r");
	char temp[20];


	while(fgets(buffer, 30, file)!=NULL){
		if(strncmp(buffer, "ROOM NAME", 9) == 0){
			newLine = strchr(buffer, '\n');
			if(newLine) *newLine = '\0';
			strcpy(rooms[index].roomName, buffer + 11);	
		}
		else if(strncmp(buffer, "ROOM TYPE", 9) == 0){
			newLine = strchr(buffer, '\n');
			if(newLine) *newLine = '\0';
			strcpy(rooms[index].roomType, buffer + 11);
			if(strcmp(rooms[index].roomType, "START_ROOM") == 0) start_room = index;
			else if(strcmp(rooms[index].roomType, "END_ROOM") == 0) end_room = index;
		}
	}
}
/*
void readFiles(){
	int i;
	int index = 0;
	FILE *file;
	char buffer[30];
	char* newLine;

	//read in files and get indices of beginning room and end room
	for(i = 0; i < 10; i++){
		if(access(roomNames[i], F_OK) != -1){
			printf("Current Index: %d\n", index);
			file = fopen(roomNames[i], "r");
			char temp[20];
			int counter = 0;
			//read into buffer 9 characters representing 'ROOM NAME'
			while(fgets(buffer, 30, file)!=NULL){
				if(strncmp(buffer, "ROOM NAME", 9) == 0){
					if(index > 0) printf ("Check Prev: %s\n", rooms[index-1].roomName);
					newLine = strchr(buffer, '\n');
					if(newLine) *newLine = '\0';
					printf("Buffer: %s\n", buffer);

					strcpy(temp, buffer + 11);
					rooms[index].roomName = temp;
					printf("Added: %s | Index: %d\n", rooms[index].roomName, index);
					
					if(index > 0) printf("Prev: %s\n", rooms[index-1].roomName);
					printf("\n");
				}
				else if(strncmp(buffer, "ROOM TYPE", 9) == 0){
					strcpy(temp, buffer + 11);
					newLine = strchr(temp, '\n');
					if(newLine) *newLine = '\0';
					rooms[index].roomType = temp;
					if(strcmp(temp, "START_ROOM") == 0) start_room = index;
					else if(strcmp(temp, "END_ROOM") == 0) end_room = index;
					
					if(index > 0) printf("Prev Type: %s\n\n", rooms[index -1].roomName);
				}
				else{
					strcpy(temp, buffer + 12);
					newLine = strchr(temp, '\n');
					if(newLine) *newLine = '\0';
					rooms[index].connected[counter] = temp;
					counter++;
				}	
				
			}
			//printf("Room Name: %s\n", rooms[index].roomName);
			//printf("Room Type: %s\n", rooms[index].roomType);
			index++;
			fclose(file);
		}	
	}
	int k;
	for(k = 0; k < index; k++){
		printf("the room name: %s\n", rooms[k].roomName);
	}
}
*/	
int roomExists(char *input, struct Room* room){
	int i;
	int bool = 0;
	printf("name to search: %s | second: %s\n",room->connected[0], room->connected[1]);
	printf("received input: %s\n", input);

	char *temp = "japan";	
	for(i = 0; i < room->numConnections; i++){
		if(strcmp(temp, input)==0){
			bool = 1;
			printf("success\n");
			break;
		}
	}
	return bool;
}
void playGame(){
	int won = 1;
	int i;
	size_t bufferSize = 0;

	struct Room* current = &rooms[start_room];
	while(won){
		char* input = NULL;
		char* newLine;
		printf("CURRENT LOCATION: %s\n", current->roomName);
		printf("POSSIBLE CONNECTIONS: ");
		for(i = 0; i < current->numConnections; i++){
			if(i == current->numConnections - 1) printf("%s.\n", current->connected[i]);
			else printf("%s, ", current->connected[i]);
		}
		printf("WHERE TO? >");
		getline(&input, &bufferSize, stdin);
	
		newLine = strchr(buffer, '\n');
		if(newLine) *newLine = '\0';
		int testcheck = roomExists(input, current);
		printf("result of checking room: %d\n", testcheck);
		if(testcheck){
			//get the roomi
			printf("got it\n");
		}	
		else{
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}
		free(input);
	}	
}

int main(){
	chdir("ohashid.rooms.7420");
	sendToRead();
/*	int i;
	for(i = 0; i < MAX_ROOMS; i++){
		printf("Room Name: %s\n", rooms[i].roomName);
		printf("Room Type: %s\n", rooms[i].roomType);
	}	
*/
	struct Room *test = getRoom("BedTwo");
	test->numConnections = 2;
	//test->connected[0] = "germany";
	test->connected[1] = "tazmania";
	rooms[1].connected[0] = "japan";
	playGame();
	
	return 0;
}

