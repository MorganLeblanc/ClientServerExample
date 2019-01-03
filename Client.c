//
// Created by quinsar on 2/27/18.
//

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int timer;
int timerTime = 30;
int port = 5000;
char* ipAddress = "127.0.0.1";

int main(int argc, char *argv[])
{
    //Check if ini file was provided
    if(argc > 1)
    {
        printf("Setting values to ini options.\n");
        FILE * ini = fopen(argv[1], "r");
        if(ini == NULL) {
            perror("Error opening file");
            return(1);
        }
        for(int i = 0; i < 4; i++)
        {
            if(i == 0)
            {
                char temp[100];
                fgets(temp, 100 * sizeof(char), ini);
            }
            else if(i == 1)
            {
                char temp[100];
                fgets(temp, 100 * sizeof(char), ini);
                char * token = strtok(temp, " =\n");
                token = strtok(NULL, " =\n");
                ipAddress = malloc(sizeof(token));
                sprintf(ipAddress, "%s", token);
                printf("%s", ipAddress);
            }
            else if(i == 2)
            {
                char temp[100];
                fgets(temp, 100 * sizeof(char), ini);
                char * token = strtok(temp, " =");
                token = strtok(NULL, " =");
                token = strtok(token, "\n");
                port = atoi(token);
                printf("port number set to %d\n", port);

            }
            else
            {
                char temp[100];
                fgets(temp, 100 * sizeof(char), ini);
                char * token = strtok(temp, " =");
                token = strtok(NULL, " =");
                token = strtok(token, "\n");
                timerTime = atoi(token);
                printf("timeout set to %d\n", timerTime);
            }

        }
    }

        int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0',sizeof(recvBuff));

        int isConnected = 0;

        timer = timerTime;

        while(isConnected == 0) {
            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                timer--;
                if (timer <= 0) {
                    printf("Could not connect to server\n");
                    return 1;
                }
                sleep(1);
            } else {
                isConnected = 1;
            }
        }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, ipAddress, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

     isConnected = 0;

        timer = timerTime;

    while(isConnected == 0) {
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            timer--;
            if(timer <= 0)
            {
                printf("Could not connect to server\n");
                return 1;
            }
            sleep(1);
        }
        else
        {
            isConnected = 1;
        }
    }

    while(1)
    {
        read(sockfd, recvBuff, sizeof(recvBuff) - 1);

        //Create a temp char to store current value and a string to store relevant values
        char temp = recvBuff[0];
        char inputString[1025];
        int rCounter = 0;

        //Loop through characters until n is found
        while (temp != 'n') {
            inputString[rCounter] = temp;
            rCounter++;
            temp = recvBuff[rCounter];
        }

        inputString[rCounter] = '\0';

        rCounter++;
        temp = recvBuff[rCounter];

        //Set target number to factor
        int targetNumber = atoi(inputString);

        printf("Received number %d from server.\n", targetNumber);

        //Loop through characters until p is found
        char primeString[1024];
        int primeCounter = 0;

        while (temp != 'p') {
            primeString[primeCounter] = temp;
            primeCounter++;
            rCounter++;
            temp = recvBuff[rCounter];
        }

        primeString[primeCounter] = '\0';

        int primeFactor = atoi(primeString);

        printf("Received prime number %d from server.\n", primeFactor);

        int factorCount = 0;
        int tempTarget = targetNumber;

        if (primeFactor == 0) {
            printf("Error passing prime number");
            return 0;
        }

        while (tempTarget % primeFactor == 0) {
            tempTarget = tempTarget / primeFactor;
            factorCount++;
        }

        printf("Prime number %d goes into %d %d times.\n", primeFactor, targetNumber, factorCount);

        //Send answer back to server
        char sendBuff[1025];

        snprintf(sendBuff, sizeof(sendBuff), "%df", factorCount);
        send(sockfd, sendBuff, strlen(sendBuff), 0);

        printf("Result sent to server\n");

    }
    return 0;
}