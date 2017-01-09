		CSCE 3600 Major Assignment 2

Description:
-----------------------------------------------------------------------
 This program was created to support and client/server and client. client 
 model using sockets to recieve and organize census data and create a
 census system using the apollo server instead of the standard CSCE machines. 
 svrMajor2.c recieves values from two clients and adds those value to the 
 current total. Then the server returns that current value to the clients. 
 cliMajor.c has users manually enter values to be sent to the server. Each 
 client is built to monitor CPU usage and cannot pass the maximum CPU threshold 
 and if so, the client will send the sum of the values to the other client and 
 proceed to disconnect from the server. The client is disconnected if the 
 client sends a zero value to the server.

File List
-----------------------------------------------------------------------
 svrMajor2.c
 cliMajor2.c
 README.md
 Makefile

Compiling the assignment
-----------------------------------------------------------------------
 server:
  compile: 
   gcc svrMajor2.c -o server
  run;
   ./server [ port ]

 client 1:
  compile:
   gcc cliMajor2.c -o client1:
  run:
   ./client1 [ rem_ipaddr ] [ cli1_port ] [ svr_port ] [ cli2_port ] [ cpu % ]

 client 2:
  compile:
   gcc cliMajor2.c -o client2:
  run:
   ./client2 [ rem_ipaddr ] [ cli2_port ] [ svr_port ] [ cli1_port ] [ cpu % ]

Instructions
-----------------------------------------------------------------------
 Unzip files
 Use compilation instructions
 enter " 0 " to exit.

Design Overview
-----------------------------------------------------------------------
 The program design consists of values being parsed into the main and handles 
 accordingly. Both client addresses and census sum is handled with global 
 variables. Strings that are created by the client are sent to the server to 
 be added to the sum and thent sent back to the client. The deisgn was fairly 
 simple since all we had to do was send and recieve messages between client/server 
 and client/client. The client side was a bit more difficult as we not only needed 
 to send and recieve messages, but also keep track of the CPU % and make sure it 
 does not reach a threshold specified.

