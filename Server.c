#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

int * primeList;
int primeCounter = 0;
int questionNumber;
int primeOut;
int factorAmount = 0;
int primeChain = 0;
int primeLimit = 1;
int isRunning = 1;
int getNext = 1;
int port = 5000;
pthread_mutex_t primeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t countMutex = PTHREAD_MUTEX_INITIALIZER;
char* factorization[1024];
int stringCount = 0;
int outputPower[1000000];

int generateBase()
{
    //Seed random number generator and then generate base in range 2 to 10
    int base = (rand() % (10 + 1 - 2) + 2);
    printf("\n\nServer has chosen to generate a number in base %d\n", base);
    return base;
}
 
int getNextNumber(int targetBase)
{
	//Determine how many digits the character count is.
    int digitCount = 1;

	//increase digit count until the number is larger than the ceiling
	while(pow((double)targetBase, (double)digitCount) < 100000)
	{
		digitCount++;
	}

	char * baseString = malloc(digitCount * sizeof(char));

	for(int i = 0; i < digitCount; i++)
	{
        int temp = rand() % targetBase;
        char tempDigit = temp + '0';
		baseString[i] = tempDigit;
	}

    double baseInt = atof(baseString);

	printf("The server has generated the number %f in base %d.\n", baseInt, targetBase);

    //declare variables to convert the number to decimal
    int decimalOutput = 0;
    int power = digitCount - 1;

    //Convert each digit into it's decimal value
    for(int i = 0; i < digitCount; i++)
    {
        char temp = baseString[i];
        int placeValue = temp - '0';
        decimalOutput = (decimalOutput + (placeValue * pow(targetBase, power)));
        power--;
    }

	printf("Server has converted number to decimal. The number is now: %d. \n", decimalOutput);

    questionNumber = decimalOutput;

    return decimalOutput;
}

int * makePrimeList(int target)
{
	//Create a list for the sieve to use
	primeList = malloc(target * sizeof(int));
	
	//set initial valeus of array
	for(int i = 0; i < target; i++)
	{
	    primeList[i] = 1;
	}

	//Set initial prime values
	primeList[0] = 0;
	primeList[1] = 0;

	//Sieve
	for(int i = 0; i * i <= target; i++)
	{
		//Check if value at i is true
		if(primeList[i])
		{
			//if yes, remove all of it's multiples
			for(int j = 2; j * i  <= target; j++)
			{
				primeList[j * i] = 0;
			}
		}
		else
		{

		}
	}

    printf("Server has generated list of primeList to be checked as factors of %d.\n", target);

	return primeList;
}

int getNextPrime()
{
    primeList[0] = 0;
    primeList[1] = 0;
    for(primeCounter; (primeCounter * 2) <= questionNumber; primeCounter++)
    {
        if(primeList[primeCounter] != 0)
        {
            primeCounter++;
            return primeCounter - 1;
        }
        else{}
    }

    while(primeOut != 0)
    {
        sleep(1);
    }

    if(factorAmount == 0)
    {
        printf("\n%d is prime!", questionNumber);
        primeChain++;
        if(primeChain >= primeLimit)
        {
            printf("Primes found! Server closing.\n");
            exit(0);
        }
    } else{
        factorAmount = 0;
        primeChain = 0;

    //Print the factorization
    printf("\n%d = 1", questionNumber);

    for(int i = 0; i < 1000000; i++)
    {
        if(outputPower[i] == 0)
        {

        }
        else
        {
            printf(" * %d^%d", i, outputPower[i]);
        }
    }
    }

    memset(outputPower, 0, sizeof(outputPower));
    primeCounter = 2;

    //Generate the new base
    int currentBase = generateBase();

    //Create the number in that base
    getNextNumber(currentBase);

    //Generate next prime and return
    int newPrime = getNextPrime();
    return newPrime;
}

void *clientHandeler(void *socketPointer)
{
    printf("\nNew client detected, connecting on new Thread.\n");

    char sendBuff[1025];

    memset(sendBuff, '0', sizeof(sendBuff));

    int *connfd_pointer = (int*)socketPointer;

    int connfd = *connfd_pointer;

    while(isRunning!=0) {
        pthread_mutex_lock(&primeMutex);
        //get next prime
        int tempPrime = getNextPrime();
        pthread_mutex_unlock(&primeMutex);

        snprintf(sendBuff, sizeof(sendBuff), "%dn%dp", questionNumber, tempPrime);
        if(!write(connfd, sendBuff, strlen(sendBuff)))
        {
        }

        pthread_mutex_lock(&countMutex);
        primeOut++;
        pthread_mutex_unlock(&countMutex);

        //Listen for servers response
        char recvBuff[1024];

        int result = recv(connfd, recvBuff, 1024, 0);

        pthread_mutex_lock(&countMutex);
        primeOut--;
        pthread_mutex_unlock(&countMutex);

        recvBuff[result] = '\0';

        int power = atoi(recvBuff);

        if (power != 0) {
            pthread_mutex_lock(&countMutex);
            factorAmount++;
            outputPower[tempPrime] = power;
            pthread_mutex_unlock(&countMutex);
        }

        memset(sendBuff, 0, strlen(sendBuff));
    }

    close(connfd);
}

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
                port = atoi(token);
                printf("port number set to %d\n", port);
            }
            else if(i == 2)
            {
                char temp[100];
                fgets(temp, 100 * sizeof(char), ini);

            }
            else
            {
                char temp[100];
                fgets(temp, 100 * sizeof(char), ini);
                char * token = strtok(temp, " =\n");
                token = strtok(NULL, " =\n");
                primeLimit = atoi(token);
                printf("target primes set to %d\n", primeLimit);
            }

        }
    }

    srand(time(0));
    //Generate initial conditions
    memset(outputPower, 0, sizeof(outputPower));
	//Generate the base
	int currentBase = generateBase();

	//Create the number in that base
	getNextNumber(currentBase);

	//Create the list of prime using the next number
	makePrimeList(1000000);

    //Set up the server
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    while(isRunning != 0)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        pthread_t client_thread;

        if(pthread_create(&client_thread, NULL, clientHandeler, &connfd)) {

            fprintf(stderr, "Error creating thread\n");
            return 1;

        }

        pthread_detach(client_thread);

    }

    return 0;
}