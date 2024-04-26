/****** Server ******/

/*
    P2_UDP_Server.c is to be run on the zeus.cs.txstate.edu server
    Compile it through the command line with $gcc -o s P2_UDP_Server.c
    run it with $./s 8000 
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>

//student struct for database
struct Student {
    
    int ID;
    char Fname[10];
    char Lname[10];
    int score;
};

int main(int argc, char **argv){

    return 0;
}
