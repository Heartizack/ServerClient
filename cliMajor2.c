/*
        CSCE 3600: Systems Programming
        Minor Assignment #7 : Sockets & Syncronization 
        Due: 11:59 PM on Wednesday, December 7, 2016 
        Professor: Dr. Mark A. Thompson, Sr
        source: cliMajor2.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int cpuThreshold; // my cpu threshold
int client1Port;  // me
int client2Port;  // other client port
char otherClientAddress[20]; // other client address

// take any errors : print and exit
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// get CPU Usage percent 0 -> 100 %
// ref: http://stackoverflow.com/questions/3769405/determining-cpu-utilization
float getCpuUsage()
{
	long double a[4], b[4], loadavg;
	FILE *fp;

	fp = fopen("/proc/stat","r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
        fclose(fp);
        sleep(1);

        fp = fopen("/proc/stat","r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
        fclose(fp);

        loadavg = 100.0 * ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
        return( loadavg );
}


// handle a message from the other client
void receiveOtherClientRequest( int myPort )
{

     int sockfd, portno;
     socklen_t clilen;

     int ns1;           // Socket for first connection. 
     int nread;         // # chars on read()
     char buf[20];      // buffer to receive message from other client

     struct sockaddr_in serv_addr, cli_addr;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = myPort;
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
	int on=1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
     // bind the  socket to the address
     if ( bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");

     // listen for connections
     if ( listen(sockfd, 5) < 0 )
              error("listen");
     clilen = sizeof(cli_addr);

     // accept connection to 1st client
     ns1 = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (ns1 < 0)
          error("ERROR on accept");
     
     printf("Connection Accepted\n");
     printf("Handler Assigned\n");
              
     bzero(buf,20);
     // receive request from client 1
     nread = recv(ns1, buf, sizeof(buf), 0);
     if (nread < 1)
	error("ERROR on read");

     int intPassed = atoi(buf);
     printf("Received %d from Other Client\n", intPassed);
     close(ns1);
     close(sockfd);
     printf("Client Disconnected\n");     

}

// send a message to the other client
void sendOtherClient(int count, int port, char *address)
{

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    portno = port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(address);
    if (server == NULL) {
        error("ERROR, no such host\n");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    // connect to other client
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
    }
    else
    {
        char buf[20];
        sprintf(buf,"%d",count);
	// send data to other client
        n = write(sockfd, buf, strlen(buf));
        if (n < 0)
            error("ERROR writing to socket");
    }
    close(sockfd);
}


// start of code (client)
int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc != 6) {
       fprintf(stderr,"usage %s <rem_ipaddrs> <svr_port> <cli1_port> <cli2_port cpu_%%> \n", argv[0]);
       exit(0);
    }
    // get arguments
    portno = atoi(argv[2]);
    client1Port = atoi(argv[3]);
    client2Port = atoi(argv[4]);
    cpuThreshold = atoi(argv[5]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    int censusTotal;
    int myCensusTotal=0;
    int censusData;
    int clientNumber;
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    {
        error("ERROR connecting");
    }
    else
    {
	printf("Connected\n");
	bzero(buffer, 256);
	n = read(sockfd, buffer, 255);

        if (n < 0)
                error("ERROR reading from socket");
        else
                sscanf(buffer,"%d %s", &clientNumber, otherClientAddress);

	int otherClientPort =
		clientNumber == 1 ? client2Port : client1Port;

	int myClientPort = 
                clientNumber == 1 ? client1Port : client2Port;

	if(fork() == 0) // if child
	{ 
		receiveOtherClientRequest(myClientPort);
		return 0;
	}

        do
	{
		printf("Enter CLIENT %d Data: ", clientNumber);
		scanf("%d", &censusData);
		myCensusTotal += censusData; // sum up my own census Data
        	bzero(buffer, 256);
		sprintf(buffer,"%d", censusData);	
		if ( censusData == 0 ) 
		{
			// user wants to quit ... breat out and disconnect
			break;
		}

		// check if threshold has been reached
                long double cu = getCpuUsage();
                int icu = (int) cu;
                if ( icu >= cpuThreshold ) 
		{
 			printf("CPU Utilization:  %.2Lf%% Threshold %d%% Exeeded\n", cu, cpuThreshold );
			printf("Sending %d to CLIENT %d\n", censusTotal, clientNumber == 1 ? 2 : 1 );
			sendOtherClient(censusTotal + censusData - myCensusTotal, otherClientPort, otherClientAddress);
			break; // done.. disconnect
		}

		// send census datadata to server 
        	n = write(sockfd, buffer, strlen(buffer));
        	if (n < 0)
                	error("ERROR writing to socket");

        	// get census total from server 
        	bzero(buffer,256);
        	n = read(sockfd, buffer, 255);
        	if (n < 0)
        	{
                	error("ERROR reading from socket");
        	}
        	else
        	{
                	censusTotal = atoi(buffer);
                	printf("SERVER Total: %d\n", censusTotal);
        	}

	} while (censusData != 0);
 
    } // else
    printf("CLIENT %d Disconnecting...\n", clientNumber);
    close(sockfd);
    return 0;
}


