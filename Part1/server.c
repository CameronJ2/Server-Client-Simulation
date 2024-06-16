#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

void stripSpaces(char *str) {
    // Find the first non-space character
    char *start = str;
    while (*start && isspace(*start)) {
        start++;
    }
    // Find the last non-space character
    char *end = str + strlen(str) - 1;
    while (end > start && isspace(*end)) {
        end--;
    }
    // Null-terminate the new string as strings need to be null terminated in c
    *(end + 1) = '\0';
    // Copy the non-space part to the beginning of the original string
    memmove(str, start, end - start + 2);
}

int clientIndexFinder(char str[], char clientPID[100][30]){
   int clientIndex = -1;
   for (int i = 0; i < 100; i++){
      if (strcmp(str, clientPID[i]) == 0){
         clientIndex = i;
      }
   }
   return clientIndex;
}

int freeIndexFinder(char clientPID[100][30]){
   int freeIndex = -1;
   for (int i = 0; i < 100; i++){
      if (strcmp("", clientPID[i]) == 0){
         freeIndex = i;
      }
   }
   return freeIndex;
}


int main() {
   int fdServer; //declare file descriptor for where the server will read from the client
   char serverFifo[] = "./tmp/serverFifo";
   int fdClient[100] = {0}; // declare file descriptor for where the client will read from the server
   char clientPID[100][30] = {0}; 
   char clientFifo[100][30] = {0};
   char receivedInfo[4][1024];
   char * token;
   mkfifo(serverFifo, 0666);
   fdServer = open(serverFifo, O_RDONLY); //busy-wait reading from the FIFO for a new request.
   while (1){ //infinite loop
      // Wait for input from any connected process
      char buffer[100] = {0};
      ssize_t bytesRead = read(fdServer, buffer, sizeof(buffer) - 1);
      if (bytesRead <= 0) {
         // No data received, so a process closed the write end of the file and we need to wait for a new one to open it
         continue;
      }
      printf("Waiting for system call...\n");
      buffer[bytesRead] = '\0'; //null terminate to make it a string
      printf("\n");
      token = strtok(buffer, ","); //take first token (pid of client)
      int token_count = 0;
      while (token != NULL && token_count < 5){ //the largest set of data recieved from 5 points, separated by commas, so token count shouldn't be greater than 5
         strcpy(receivedInfo[token_count], token); //max token length is 100, so we subtract 1 for the null byte at the end.
         receivedInfo[token_count][1024 - 1] = '\0'; // Null-terminate the string
         // Get the next token
         token = strtok(NULL, ",");
         token_count++;
      }
      for (int i = 0; i < 5; i++) {
         stripSpaces(receivedInfo[i]);
      }
      
      // input is recieved and parsed, now to run the logic for what we do with the information.
      if (strcmp(receivedInfo[1], "1") == 0){ //system call 1. strcmp evaluates to 0 if both strings are the same
         printf("Received a system call 1\n");
         int freeIndex = freeIndexFinder(clientPID);
         strcpy(clientPID[freeIndex], receivedInfo[0]);
         stripSpaces(clientPID[freeIndex]);
         strcpy(clientFifo[freeIndex], receivedInfo[3]);
         stripSpaces(clientFifo[freeIndex]);
         fdClient[freeIndex] = open(clientFifo[freeIndex], O_WRONLY);
         for (int i = 0; i < 5; i++) {
         printf("%s\n", receivedInfo[i]);
         }
      }

      else if (strcmp(receivedInfo[1], "2") == 0 || strcmp(receivedInfo[1], "3") == 0){ //system call 2 or 3. strcmp evaluates to 0 if both strings are the same
         printf("Received a system call 2 or 3\n");
         char clientNumber[1024];
         strcpy(clientNumber, receivedInfo[0]);
         int indexOfClient = clientIndexFinder(clientNumber, clientPID);
         char clientString[1024];
         strcpy(clientString, receivedInfo[4]);
         int counter = 0;
         for (int i = 0; i < strlen(clientString); i++){
            if (tolower(clientString[i]) == 'a' || tolower(clientString[i]) == 'e' || tolower(clientString[i]) == 'i' || tolower(clientString[i]) == 'o' || tolower(clientString[i]) == 'u'){
               counter++;
            }
         }
         char returnString[50];
         if (strcmp(receivedInfo[1], "2") == 0){ //system call 2
            printf("it was a 2\n");
            sprintf(returnString, "The number of vowels in the word %s is: %d\n", clientString, counter);
            write(fdClient[indexOfClient], returnString, strlen(returnString)+1);
            for (int i = 0; i < 5; i++) {
            printf("%s\n", receivedInfo[i]);
         }
         }
         else{ //system call 3
            printf("it was a 3\n");
            int numNonVowels = strlen(clientString) - counter;
            sprintf(returnString, "The number of non-vowels in the word %s is: %d\n", clientString, numNonVowels);
            write(fdClient[indexOfClient], returnString, strlen(returnString)+1);
            for (int i = 0; i < 5; i++) {
            printf("%s\n", receivedInfo[i]);
         }
         }
      }

      else if (strcmp(receivedInfo[1], "0") == 0){ //system call 0
         printf("Received a system call 0\n");
         char clientNumber[1024];
         strcpy(clientNumber, receivedInfo[0]);
         int indexOfClient = clientIndexFinder(clientNumber, clientPID);
         close(fdClient[indexOfClient]);
         //reset the values that the index was associated with to 0
         memset(clientFifo[indexOfClient], 0, sizeof(clientFifo[indexOfClient]));
         memset(clientPID[indexOfClient], 0, sizeof(clientPID[indexOfClient]));
         fdClient[indexOfClient] = 0;
         for (int i = 0; i < 5; i++) {
         printf("%s\n", receivedInfo[i]);
         }
      }

      else if (strcmp(receivedInfo[1], "-1") == 0){
         printf("Received a system call -1\n");
         for (int i = 0; i < 5; i++) {
         printf("%s\n", receivedInfo[i]);
         }
         close(fdServer);
         break;
      }
    }
   return 0;
}


