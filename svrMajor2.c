/*      CSCE 3600: Systems Programming
        Major Assignment #2 : Sockets & Syncronization 
        Due: 11:59 PM on Wednesday, December 7, 2016
        Professor: Dr. Mark A. Thompson, Sr
        source: svrMajor2.c
*/

//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

// Census Total
int censusTotal=0;
// Capture client IP address when then connect
char client1Addr[40];
char client2Addr[40];

// shared buffer
char buffer[256];

// take any errors : print and exit
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// main : start of server code execution
int main(int argc, char *argv[])
{
     int sockfd, portno;
     socklen_t clilen;

     int ns1;           /* Socket for first connection. */
     int ns2;           /* Socket for second connection. */
     int maxfd;         /* descriptors up to maxfd-1 polled*/
     int nread;         /* # chars on read()*/
     fd_set fds;        /* Set of file descriptors to poll*/

     struct sockaddr_in serv_addr, cli_addr;
     if (argc != 2) {
	error("usage: svrMajor2 <port>\n");
     }

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
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

     printf("Waiting for Incoming Connections...\n");

     // accept connection to 1st client
     ns1 = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (ns1 < 0) 
          error("ERROR on accept 1");

     //sprintf(client1Addr, "%s", inet_ntoa(cli_addr.sin_addr));
     strcpy(client1Addr, "192.168.122.106");

     printf("Client Connection Accepted\n");
     printf("Client Handler Assigned\n");

     // accept connection to 2nd client
     ns2 = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (ns2 < 0)
          error("ERROR on accept 2");
     
     //sprintf(client2Addr, "%s", inet_ntoa(cli_addr.sin_addr));
     strcpy(client2Addr, "192.168.122.125");

     printf("Client Connection Accepted\n");
     printf("Client Handler Assigned\n");

     // send client number and client address
     sprintf(buffer, "%d %s", 1, client1Addr);
     send(ns1, buffer, strlen(buffer), 0);

     // send client nubber and client address
     sprintf(buffer, "%d %s", 2, client2Addr);
     send(ns2, buffer, strlen(buffer), 0);

     int client1done = 0;
     int client2done = 0;

     maxfd = (ns1 > ns2 ? ns1: ns2) + 1;
     // int to receive converted value sent from either client
     int censusCount;
     while (1)
     {
                /* Set up polling using select. */
                FD_ZERO(&fds);
                FD_SET(ns1,&fds);
                FD_SET(ns2,&fds);

                /* Wait for some input. */
                select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0,
                                (struct timeval *) 0);

		bzero(buffer, 256); // clear the buffer 

                /* If either descriptor has some input,
                   read it and process the request. */
                if( client1done == 0 && FD_ISSET(ns1, &fds) )
                {
			// receive request from client 1
                        nread = recv(ns1, buffer, sizeof(buffer), 0);
                        /* If error or eof, client 1 is done. */
                        if(nread < 1) 
                        {
			  client1done = 1;
	                  printf("Client Disconnected\n");
			}
			else
			{
			  // get censusCount from client
                          censusCount = atoi( buffer );
			  censusTotal += censusCount;
			  sprintf(buffer, "%d", censusTotal);
                          printf("%d\n", censusTotal);
                          send(ns1, buffer, strlen(buffer), 0);
			}
                }

                if( client2done == 0 &&  FD_ISSET(ns2, &fds) )
                {
			// recieve request from client 2
                        nread = recv(ns2, buffer, sizeof(buffer), 0);
                        /* If error or eof, client 2 is done. */
                        if(nread < 1)
                        {
			  client2done = 1;
			  printf("Client Disconnected\n");
                        }
                        else
                        {
                          // get censusCount from client
                          censusCount = atoi( buffer );
                          censusTotal += censusCount;
                          sprintf(buffer, "%d", censusTotal);
                          printf("%d\n", censusTotal);
                          send(ns2, buffer, strlen(buffer), 0);
                        }
                }
     }
    
     // close sockets
     close(ns1);
     close(ns2);
     close(sockfd);

     return 0; 
}


