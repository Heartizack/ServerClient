################################################################ 
# 
# shell.c is the name of your source code; you may change this. 
# However, you must keep the name of the executable 
#    cliMajor7 and svrMajor7. 
# 
# Type "make" or "make cliMajor7" to compile your code 
# 
# Type "make clean" to remove the executable (and object files) 
# 
################################################################ 
CC=gcc 
CFLAGS=-Wall -g 

all: cliMajor2.c svrMajor2.c
	$(CC) -o client $(CFLAGS) cliMajor2.c
	$(CC) -o server $(CFLAGS) svrMajor2.c

cliMajor2: cliMajor2.c
	$(CC) -o client $(CFLAGS) cliMajor2.c

svrMajor2: svrMajor2.c
	$(CC) -o server $(CFLAGS) svrMajor2.c

clean:        
	$(RM) client server  


