#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

int main(){
    pid_t pid = getpid();
    char pidString[100];
    //snprintf(pidString, sizeof(pidString), "%d", pid);
    sprintf(pidString,"%d", pid);
    printf("Process ID as a string: %s\n", pidString);
    char serverFifo[] = "./tmp/serverFifo";
    printf("This should get read ALWAYS!\n");
    int fdServer = open(serverFifo, O_WRONLY);
    printf("This should still get read only after both server and client are connected through the server FIFO!!\n");
    char clientFifo[100];
    sprintf(clientFifo, "./tmp/%sFifo", pidString);
    mkfifo(clientFifo, 0666);
    int fdClient;
    //int fdClient = open(clientFifo, O_RDONLY);
    //int hasConnectedToServer = 0;
    int systemCallNumber;
    //int numberOfParameters;
    //int valuesOfParameters;

    while (1){
        printf("What server call would you like to make? (-1 = terminate server and this client, 0 = terminate this client, 1 = establish connection with server, 2 = number of vowels, 3 = number of non-vowels, 4 = request semaphore for critical section, 5 = send a message to server, 6 = receive a message from server): ");
        scanf("%d", &systemCallNumber);

        if (systemCallNumber == -1){
            char message[100];
            sprintf(message, "%s, -1, , , ", pidString);
            write(fdServer, message, strlen(message));
            close(fdServer);
            close(fdClient);
            break;
        }
        else if (systemCallNumber == 0){
            char message[100];
            sprintf(message, "%s, 0, , , ", pidString);
            write(fdServer, message, strlen(message));
            sleep(1);
            close(fdServer);
            close(fdClient);
            break;
        }
        else if (systemCallNumber == 1){
            char message[100];
            sprintf(message, "%s, 1, 1, %s, ", pidString, clientFifo);
            write(fdServer, message, strlen(message));
            fdClient = open(clientFifo, O_RDONLY);
            printf("Server and client both completely hooked up!\n");
        }
        else if (systemCallNumber == 2){
            char myWord[1000];
            printf("What word do you want to find the number of vowels for? (no more than 50 characters, and no spaces): ");
            //fgets(myWord, sizeof(myWord), stdin);
            scanf("%s", myWord);
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("this is the word that's supposed to be sent: %s\n", myWord);
            char message[1000];
            sprintf(message, "%s, 2, 2, %lu, %s", pidString, strlen(myWord), myWord);
            printf("writing to server...\n");
            write(fdServer, message, strlen(message));
            printf("write complete!\n");
            char buffer[1000] = {0};
            printf("reading from server...\n");
            ssize_t bytesRead = read(fdClient, buffer, sizeof(buffer) - 1);
            printf("read complete!\n");
            buffer[bytesRead] = '\0';
            printf("%s", buffer);
        }
        else if (systemCallNumber == 3){
            char myWord[1000];
            printf("What word do you want to find the number of non-vowels for? (no more than 50 characters): ");
            //fgets(myWord, sizeof(myWord), stdin);
            scanf("%s", myWord);
            //getchar();
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("this is the word that's supposed to be sent: %s\n", myWord);
            char message[100];
            sprintf(message, "%s, 3, 2, %lu, %s", pidString, strlen(myWord), myWord);
            printf("writing to server...\n");
            write(fdServer, message, strlen(message));
            printf("write complete!\n");
            char buffer1[1000] = {0};
            printf("reading from server...\n");
            ssize_t bytesRead1 = read(fdClient, buffer1, sizeof(buffer1) - 1);
            printf("read complete!\n");
            buffer1[bytesRead1] = '\0';
            printf("%s", buffer1);
        }
        else if (systemCallNumber == 4){ //request the semaphore
        char message[100];
        sprintf(message, "%s, 4, 1, Requesting semaphore, 1", pidString);
        printf("Executing non-critical section...\n");
        //non-critical section
        for (int i = 1; i <= 20; i++){
            printf("Executing non-critical section: %d/20\n", i);
            sleep(1);
        }
        printf("Critical section encountered!\n");
        printf("Requesting access to critical section...\n");
        write(fdServer, message, strlen(message));
        printf("Request sent! Waiting for access...\n");
        char buffer2[1000] = {0};
        ssize_t bytesRead2 = read(fdClient, buffer2, sizeof(buffer2) - 1);
        printf("Access granted!\n");
        buffer2[bytesRead2] = '\0';
        printf("message from server: %s\n", buffer2);
        //critical section
        for (int i = 1; i <= 20; i++){
            printf("Executing critical section: %d/20\n", i);
            sleep(1);
        }
        printf("Critical section complete! Retruning semaphore...\n");
        sprintf(message, "%s, 4, 1, Returning semaphore, 0", pidString);
        write(fdServer, message, strlen(message));
        }

        else if (systemCallNumber == 5){ //message send
            char message[1024];
            char type[5];
            printf("What message type are you sending? (0 = number, 1 = word, 2 = sentence): ");
            scanf("%s", type);
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("What is the message?: ");
            fgets(message, sizeof(message), stdin);
            /* c = getchar();
            while ((c = getchar()) != '\n' && c != EOF) {} */


            char writeMessage[1024];
            sprintf(writeMessage, "%s, 5, %s-%lu-%s, send message, ", pidString, type, strlen(message), message);
            write(fdServer, writeMessage, strlen(writeMessage));
            
        }

        else if (systemCallNumber == 6){
            char message[1024];
            char type[5];
            printf("What message type are you looking for? (0 = number, 1 = word, 2 = sentence): ");
            scanf("%s", type);
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("\n");
            sprintf(message, "%s, 6, %s, receive message, ", pidString, type);
            write(fdServer, message, strlen(message));
            char buffer3[1024] = {0};
            ssize_t bytesRead3 = read(fdClient, buffer3, sizeof(buffer3) - 1);
            buffer3[bytesRead3] = '\0';
            printf("message from server: \n%s\n", buffer3);
        }
            
    }
    return 0;
}

