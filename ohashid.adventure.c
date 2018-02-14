#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <dirent.h>

#define MAX_CONNECTIONS 6
#define MAX_ROOMS 7

struct Room{
	char roomName[25];
	char roomType[25];
	int numConnections;
	char connected[MAX_CONNECTIONS][25];
};

struct Room rooms[MAX_ROOMS]; //global - holds room structs, info will be read in from files
char* path[50]; //global - array of char* to hold string names of rooms visited

//global - array of char* to hold all possible room names
char* roomNames[] = {"Nursery", "BedOne", "BedTwo", "Living", "Kitchen", "Basement",
			"Attic", "Garage", "Bathroom", "Office"};

//global - ints to represent the rooms[] index of the beginning and final rooms
//along with the step counter
int start_room, end_room, connect, stepCounter;
char fileName[256]; //global - holds the fileName to be read in "ohashid.rooms. ____" 
pthread_mutex_t lock;
pthread_t thread[2];

void readFiles();
void* playGame();
void sendToRead();
void readPerFile(char*string, int);
struct Room* getRoom(char*);
int checkWin(struct Room*);
void winMessage();
void checkTime();
void getDir();
void* getTime();
void printTime();

/*
 getRoom will search for the room in the rooms array, and return it if found.
*/
struct Room* getRoom(char roomName[]){
	int i;
	//iterate from i = 0 to MAX_ROOMS (7)
	for(i = 0; i < MAX_ROOMS; i++){
		//strcmp on each room name at index i with the argument roomName[]
		if(strcmp(rooms[i].roomName, roomName)==0){
			return &rooms[i]; //return address to room if found
		}
	}	
}

/*
 sendToRead() will iterate through the 10 possible room names. If the room name exists as a file, 
 it will be passed in, along with the index of the rooms array for its contents to be instantiated.
*/
void sendToRead(){
	int i;
	int index = 0;
	//iterate through all available room names
	for(i = 0; i < 10; i++){
		//check if that room name exists as a file
		if(access(roomNames[i], F_OK) != -1){
			//if the room name file exists, pass it to readPerFile
			//with the index of the rooms[] array to be filled out
			readPerFile(roomNames[i], index);
			index++; //increment the index after returning
		}
	}
	stepCounter = 0; //intiailize the game's global variable stepCounter
}

/*
 readPerFile() will read the contents of the file into the designated room structs
 This is the source of the information required to play the game
*/
void readPerFile(char* roomName, int index){
	int i;
	FILE *file;
	char buffer[30];
	char* remove;
	
	file = fopen(roomName, "r"); //open file with argument roomName for "read"
	
	//continue to read in each line to the buffer of size 30 
	while(fgets(buffer, 30, file)!=NULL){
		//use strncmp to verify that the first 9 characters of the buffer are ROOM NAME
		if(strncmp(buffer, "ROOM NAME", 9) == 0){
			remove = strchr(buffer, '\n'); //remove the newline in the buffer
			if(remove) *remove = '\0'; //replace with null terminator
			strcpy(rooms[index].roomName, buffer + 11); //copy into roomName string past the 11th char
			rooms[index].numConnections = 0; //instantiate the connections to 0	
		}
		else if(strncmp(buffer, "ROOM TYPE", 9) == 0){ //checks if first 9 of buffer is ROOM TYPE
			remove = strchr(buffer, '\n'); //remove newline in the buffer
			if(remove) *remove = '\0'; //replace with null terminator
			strcpy(rooms[index].roomType, buffer + 11); //copy into room type buffer past the 11th char
			//check if the type is START_ROOM or END_ROOM and set the start_room/end_room index accordingly
			if(strcmp(rooms[index].roomType, "START_ROOM") == 0) start_room = index; 
			else if(strcmp(rooms[index].roomType, "END_ROOM") == 0) end_room = index;
		}
		else{ //if not ROOM NAME or ROOM TYPE, it has to be CONNECTION
			remove = strchr(buffer, '\n'); //remove newline to replace with null terminator
			if(remove) *remove = '\0';
			//get input past 14th char
			strcpy(rooms[index].connected[rooms[index].numConnections], buffer + 14); 
			rooms[index].numConnections++; //increment numConnection for that room
		}
	}
}

/*
 roomExists() will verify if the input received in playGame is valid
 This function is called from playGame when user input is requested for 'time' or a connection room name
*/
int roomExists(char *input, struct Room* room){
	int i;
	int bool = 0; //instantiate the int as 0 to default to false
	//iterate through all connections to the argument room
	for(i = 0; i < room->numConnections; i++){
		//if a room with the input name exists, set int to 1 (true) and exit out of the loop
		if(strcmp(room->connected[i], input)==0){
			bool = 1;
			break;
		}
	}
	return bool;
}

/*
 playGame() is responsible for providing the user the interface in which the game will be played
*/
void* playGame(){
	int won = 1;
	int i;
	size_t bufferSize = 0;
	struct Room* current = &rooms[start_room]; //pointer to struct Room to hold start room
	//continue to loop as long as won == 1
	while(won){
		pthread_mutex_lock(&lock);
		char* input = NULL;
		char* newLine;
		printf("CURRENT LOCATION: %s\n", current->roomName);
		printf("POSSIBLE CONNECTIONS: ");
		//iterate through sturct Room *currents connections to display to user
		for(i = 0; i < current->numConnections; i++){
			//print with a period and newline if its the last connection
			if(i == current->numConnections - 1) printf("%s.\n", current->connected[i]);
			else printf("%s, ", current->connected[i]); //otherwise, print with , and ' '
		}
		printf("WHERE TO? >"); //request user input
		getline(&input, &bufferSize, stdin); //use getLine to read input into input buffer via stdin
	
		newLine = strchr(input, '\n');
		if(newLine) *newLine = '\0'; //replace the newline with a null terminator (useful for strcmp)
		
		//initiate if the input is time
		if(strcmp(input, "time")==0){
			pthread_mutex_unlock(&lock);
			pthread_create(&thread[1], NULL, getTime, NULL);
			pthread_join(thread[1], NULL);
			pthread_mutex_lock(&lock);
			printTime();	
		}
		//otherwise, check if the input room name mexists
		else if(roomExists(input, current)){
			//get the room
			current = getRoom(input);
			printf("\n"); //spacing for formatting
			path[stepCounter] = current->roomName; //add room name to the path array
			stepCounter++; //increment step counter to hold steps and path[] index
			//pass the current room pointer to checkWin() to see if the rooms type is END ROOM
			if(checkWin(current)){
				//if type is END_ROOM won = 0 to conclude the while loop
				won = 0;
				winMessage(); //winMessage called to display final message and show path
			}
		}	
		//if the input is not a valid connection or 'time', send the following message
		else{
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}
		free(input); //free the buffer (for next input)
		pthread_mutex_unlock(&lock);
	}	
}

/*
 checkWin() will determine if the passed in room type is END_ROOM.
 Will return 1 to indicate the the room was found to trigger winMessage()
 Else, will return 0
*/
int checkWin(struct Room* room){
	int bool = 0; //instantiate bool as 0 to default false
	if(strcmp(room->roomType, "END_ROOM")==0) bool = 1; //set bool = 1 if type is END ROOM
	return bool;
}

/*
 Void function to display the winning message when END_ROOM is found.
 Function will also iterate through the paths taken and display them in sequence
*/
void winMessage(){
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepCounter);
	int i;
	//iterate through the number of steps taken to display the path taken by the user
	for(i = 0; i < stepCounter; i++){
		printf("%s\n", path[i]); //print path content at each valid index
	} 
}

/*
 Void function to retrieve directory the program was run in, and getting the file name
 of the most recently created ohashid.rooms.pid file
 This code was adopted from Brewster CS344 2.4 
*/
void getDir(){
	//create prefix target
	char filePrefix[32] = "ohashid.rooms.";
	int bestTime = -1;
	DIR* dir;
	struct stat info;
	struct dirent *file; //intended to hold the file names

	char cwd[200];
	getcwd(cwd, sizeof(cwd));
	
	//open current directory using ".", check against NULL to see if it was opened
	if((dir = opendir(cwd)) != NULL){
		//loop while file names exist
		while(file = readdir(dir)){
			//check if prefix matches any file name via stsrt()
			if(strstr(file->d_name, filePrefix)!=NULL){
				stat(file->d_name, &info); //obtain file stat information
				if((int)info.st_mtime > bestTime){ //compare new time to previous one
					//bestTime = curTime; //set newest time to current time
					bestTime = (int)info.st_mtime;
					strcpy(fileName, file->d_name); //copy the filename into filename buffer
				}
			}
		}
	}
	closedir(dir);	
}

/*
 getTime() will be responsible for reading in the current time into the currentTime.txt file
*/
void* getTime(){
	pthread_mutex_lock(&lock);

	chdir("..");
	char cwd[256];
	getcwd(cwd, sizeof(cwd));
	printf("\n");
	char* makeNull;	
	char* timeFile = "currentTime.txt";
	char timeBuffer[256];
	struct tm *sysTime;
	time_t current = time(NULL);
	
	sysTime = localtime(&current);
	//use strftime to fill the timeBuffer with the string content based on the values designated by sysTime
	strftime(timeBuffer, sizeof(timeBuffer), "%t%-I:%M%#p, %A, %B %d, %Y" , sysTime);
	
	makeNull = strchr(timeBuffer, '\0'); //replace the null terminator with a newline
	if(makeNull) *makeNull = '\n';
	FILE* newFile = fopen(timeFile, "w"); //open the file for writing
	fprintf(newFile, timeBuffer); //print the time into the opened file
	fclose(newFile); //close the file
	pthread_mutex_unlock(&lock);
}

/*
 printTime() opens "currentTime.txt" created in getTime() and reads the content
*/
void printTime(){
	FILE* newFile = fopen("currentTime.txt", "r");
	char timeBuffer[50];
	fgets(timeBuffer, 50, newFile);
	printf("%s\n", timeBuffer);
}
/*
 Driver to get the directory the necessary files are in, read the data in, and play Adventure
*/
int main(){
	pthread_mutex_init(&lock, NULL);
	getDir();
	chdir(fileName);
	sendToRead();

	pthread_create(&thread[0], NULL, playGame, NULL);
	pthread_join(thread[0], NULL);

	pthread_mutex_destroy(&lock);	
	return 0;
}

