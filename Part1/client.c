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
        printf("What server call would you like to make? (-1 = terminate server and this client, 0 = terminate this client, 1 = establish connection with server, 2 = number of vowels, 3 = number of non-vowels): ");
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
            char buffer[1000] = {0};
            printf("reading from server...\n");
            ssize_t bytesRead = read(fdClient, buffer, sizeof(buffer) - 1);
            printf("read complete!\n");
            buffer[bytesRead] = '\0';
            printf("%s", buffer);
        }
    }
    return 0;
}

