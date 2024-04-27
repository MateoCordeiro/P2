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
    grade, 
    cgrade;

char FirstName[10],
    LastName[10],
    msg[1024];

struct sockaddr_in server;

socklen_t server_address_size_socklen = sizeof(server);

struct Student {
    
    int ID;
    char FirstName[10];
    char LastName[10];
    int grade;
};

int printMenu() {
   
    int userChoice;

    printf("\n\nStudent Database Options Menu\n");
    printf("=============================\n");
    printf("1. add: Adds a student to the database. (requires:ID, FirstName, LastName, grade)\n");
    printf("2. delete(ID): Deletes a student from the database. (requires: ID)\n");
    printf("3. display(ID): Displays info about a student, given their ID (requires: ID)\n");
    printf("4. display(grade): Displays info about students with that grade. (requires: grade\n");
    printf("5. display_all: Displays info about all students in the database. (requires: N/a)\n");

    printf("6. exit:\n\n");

    printf("Select a number: ");
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

        // user input FirstName
        printf("Enter FirstName: ");
        scanf("%s", &FirstName);
        uint32_t FirstNameSize = (strlen(FirstName));
        sendto(s, &FirstNameSize, sizeof(FirstNameSize), 0, (struct sockaddr *)&server, server_address_size);
        sendto(s, FirstName, strlen(FirstName), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);

        // user input LastName
        printf("Enter LastName: ");
        scanf("%s", &LastName);
        uint32_t LastNameSize = (strlen(LastName));
        sendto(s, &LastNameSize, sizeof(LastNameSize), 0, (struct sockaddr *)&server, server_address_size);
        sendto(s, LastName, strlen(LastName), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);

        // user input grade
        printf("Enter grade: ");
        scanf("%d", &grade);
        cgrade = htonl(grade);
        sendto(s, &cgrade, sizeof(cgrade), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);

        //receive confirmation that student was added
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);
    }
    else if (userInput == 2)
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
    else if (userInput == 3)
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
            printf("%d %s %s %d\n", receivedStudent.ID, receivedStudent.FirstName, receivedStudent.LastName, receivedStudent.grade);
        }
        else
        {
            printf("Student with ID %d not found.\n", ID);
        }
    }
    else if (userInput == 4)
    {
       // user input grade
        printf("You chose display(grade), enter grade: ");
        scanf("%d", &grade);
        cgrade = htonl(grade);
        sendto(s, &cgrade, sizeof(cgrade), 0, (struct sockaddr *)&server, server_address_size);
        recvfrom(s, msg, sizeof(msg), 0, (struct sockaddr *)&server, &server_address_size);
        printf("%s\n", msg);     

        // receive and display student information from the server
        struct Student receivedStudent;
        recvfrom(s, &receivedStudent, sizeof(receivedStudent), 0, (struct sockaddr *)&server, &server_address_size);

        if (receivedStudent.ID == -2)
        {
            printf("No students found with a grade higher than %d\n", grade);
        }
        else
        {
            printf("Student Information:\n");
            while (receivedStudent.ID != -1)
            {
                printf("%d %s %s %d\n", receivedStudent.ID, receivedStudent.FirstName, receivedStudent.LastName, receivedStudent.grade);
                recvfrom(s, &receivedStudent, sizeof(receivedStudent), 0, (struct sockaddr *)&server, &server_address_size_socklen);
            }
        }
    }
    else if (userInput == 5)
    {
        // receive and display
        struct Student receivedStudent;
        recvfrom(s, &receivedStudent, sizeof(receivedStudent), 0, (struct sockaddr *)&server, &server_address_size);
        printf("Student Information:\n");
        while (receivedStudent.ID != -1)
        {
            printf("%d %s %s %d\n", receivedStudent.ID, receivedStudent.FirstName, receivedStudent.LastName, receivedStudent.grade);
            recvfrom(s, &receivedStudent, sizeof(receivedStudent), 0, (struct sockaddr *)&server, &server_address_size);
        }
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

    uint32_t num,
        cnum;


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