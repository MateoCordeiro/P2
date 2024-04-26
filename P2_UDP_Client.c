/****** Client ******/

/*
    P2_UDP_Client.c is to be run on the zeus.cs.txstate.edu server
    Compile it through the command line with $gcc -o c P2_UDP_Client.c
    run it with $./c zeus.cs.txstate.edu
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>

#include <sys/socket.h>
#include <netinet/in.h>

int printMenu();
void sendCommand(int userInput);

int s, 
    server_address_size,
    userInput, 
    cuserInput,
    ID, 
    cID,
    score, 
    cscore;

char Fname[10],
    Lname[10],
    msg[1024];

struct sockaddr_in server;

socklen_t server_address_size_socklen = sizeof(server);

struct Student {
    
    int ID;
    char Fname[10];
    char Lname[10];
    int score;
};

int printMenu() {
    
    int userChoice;

    printf("\n\n
        Student Database Options Menu\n
        =============================\n
        1. add(ID, Fname, Lname, score): this request adds new student information to the database.\n
        2. display(ID): this request sends the ID of a student to the server, and the server returns the student's information to the client.\n
        3. display(score): this request sends a score to the server, and the server returns the information of all students whose scores are above the sent score to the client.\n
        4. display_all: this request displays the information of all the students currently in the database.\n
        5. delete(ID): this request deletes the student entry with that ID from the database.\n
        6. exit: this request terminates the program.\n\n");
    
    printf("Enter the number associated with the command you wish to perform: ");
    scanf("%d", &userChoice);

    return userChoice;
}

void sendCommand(int userInput) {
    
    // send databse command attributes (if needed) from user to server
    if (userInput == 1)
    {
        // user input ID
        printf("You chose add, enter ID: ");
        scanf("%d", &ID);
        cID = htonl(ID);
        sendto(s, &cID, sizeof(cID), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);

        // user input Fname
        printf("Enter Fname: ");
        scanf("%s", &Fname);
        uint32_t FnameSize = (strlen(Fname));
        sendto(s, &FnameSize, sizeof(FnameSize), 0, (struct sockaddr *)&server, server_address_size);
        sendto(s, Fname, strlen(Fname), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);

        // user input Lname
        printf("Enter Lname: ");
        scanf("%s", &Lname);
        uint32_t LnameSize = (strlen(Lname));
        sendto(s, &LnameSize, sizeof(LnameSize), 0, (struct sockaddr *)&server, server_address_size);
        sendto(s, Lname, strlen(Lname), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);

        // user input score
        printf("Enter score: ");
        scanf("%d", &score);
        cscore = htonl(score);
        sendto(s, &cscore, sizeof(cscore), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);

        //receive confirmation that student was added
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);
    }
    else if (userInput == 2)
    {
        // user input ID
        printf("You chose display(ID), enter ID: ");
        scanf("%d", &ID);
        cID = htonl(ID);
        sendto(s, &cID, sizeof(cID), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);

        // receive student information from the server
        struct Student receivedStudent;
        recvfrom(s, &receivedStudent, sizeof(receivedStudent), 0, (struct sockaddr *)&server, &server_address_size);

        if (receivedStudent.ID != -1)
        {
            printf("Student Information:\n");
            printf("%d %s %s %d\n", receivedStudent.ID, receivedStudent.Fname, receivedStudent.Lname, receivedStudent.score);
        }
        else
        {
            printf("Student with ID %d not found.\n", ID);
        }
    }
    else if (userInput == 3)
    {   
        // user input score
        printf("You chose display(score), enter score: ");
        scanf("%d", &score);
        cscore = htonl(score);
        sendto(s, &cscore, sizeof(cscore), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);     

        // receive and display student information from the server
        struct Student receivedStudent;
        recvfrom(s, &receivedStudent, sizeof(receivedStudent), 0, (struct sockaddr *)&server, &server_address_size);

        if (receivedStudent.ID == -2)
        {
            printf("No students found with a score higher than %d\n", score);
        }
        else
        {
            printf("Student Information:\n");
            while (receivedStudent.ID != -1)
            {
                printf("%d %s %s %d\n", receivedStudent.ID, receivedStudent.Fname, receivedStudent.Lname, receivedStudent.score);
                recvfrom(s, &receivedStudent, sizeof(receivedStudent), 0, (struct sockaddr *)&server, &server_address_size_socklen);
            }
        }
    }
    else if (userInput == 4)
    {
        // receive and display
        struct Student receivedStudent;
        recvfrom(s, &receivedStudent, sizeof(receivedStudent), 0, (struct sockaddr *)&server, &server_address_size);
        printf("Student Information:\n");
        while (receivedStudent.ID != -1)
        {
            printf("%d %s %s %d\n", receivedStudent.ID, receivedStudent.Fname, receivedStudent.Lname, receivedStudent.score);
            recvfrom(s, &receivedStudent, sizeof(receivedStudent), 0, (struct sockaddr *)&server, &server_address_size);
        }
    }
    else if (userInput == 5)
    {
        // user input ID
        printf("You chose delete, enter ID: ");
        scanf("%d", &ID);
        cID = htonl(ID);
        sendto(s, &cID, sizeof(cID), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);
    }
    else
    {
        printf("That is not a valid menu selection. Please enter 1-6.");
    }
}

int main(int argc, char **argv) {

    unsigned short port;
    char buffer[2048];
    struct hostent *hostnm;

    uint32_t num, cnum;


    if ((s = socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
        printf("socket creation error");
    }

    server.sin_family = AF_INET; // Internet domain
    server.sin_port = htons(8000); // port
    hostnm = gethostbyname(argv[1]);  //get server's name
    server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr); 


    // communication starts from here
    server_address_size = sizeof(server);

    // send an integer to the server
    printf("To establish communication, enter an integer: ");
    scanf("%d", &num);

    /* htonl stands for "host to network long" and is a function used in networking applications to convert values from host byte order to network byte order.*/
    cnum = htonl(num);
    sendto(s, &cnum, sizeof(cnum), 0, (struct sockaddr *)&server, server_address_size);

    // receive a reply message from the server
    recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
    printf("%s\n", msg);

    // print menu 
    userInput = printMenu();

    // send user input
    cuserInput = htonl(userInput);
    sendto(s, &cuserInput, sizeof(cuserInput), 0, (struct sockaddr *)&server, server_address_size);

    // receive a reply message from the server
    recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
    printf("%s\n", msg);

    //while loop to get menu to re-display
    while (userInput != 6)
    {
        // send command to server
        sendCommand(userInput);

        // escape case
        userInput = printMenu();
        // send new user input
        cuserInput = htonl(userInput);
        sendto(s, &cuserInput, sizeof(cuserInput), 0, (struct sockaddr *)&server, server_address_size);

        // receive a reply message from the server
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);
    }

    close(s);

    return 0;
}