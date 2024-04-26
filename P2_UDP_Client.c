/****** Client ******/

/*
    P2_UDP_Client.c is to be run on the zeus.cs.txstate.edu server
    Compile it through the command line with $gcc -o c P2_UDP_Client.c
    run it with $./c zeus.cs.txstate.edu
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

int printMenu();

void sendCommand(int userInput);

int s, server_address_size;
struct sockaddr_in server;
int userInput, cuserInput;
int ID, cID;
char Fname[10];
char Lname[10];
int score, cscore;
char msg[1024];
socklen_t server_address_size_socklen = sizeof(server);

struct Student {
    
    int ID;
    char Fname[10];
    char Lname[10];
    int score;
};

int printMenu() {
    
    int userChoice;

    return userChoice;
}

void sendCommand(int userInput) {

}

int main(int argc, char **argv) {

    return 0;
}