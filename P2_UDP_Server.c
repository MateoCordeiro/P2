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
#include <netdb.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

//student struct for database
struct Student {
    
    int ID;
    char FirstName[10];
    char LastName[10];
    int Grade;
};

int main(int argc, char **argv){

    int s,
        namelen,
        client_address_size,
        userInput,
        ID, 
        cID,
        Grade, 
        cGrade;
    
    struct sockaddr_in server, 
        client;

    char buffer[2048],
        msg[1024],
        FirstName[10],
        LastName[10];

    uint32_t num, 
        FirstNameSize, 
        LastNameSize;

    // Create datagram socket in the internet domain. Use the UDP. 
    s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET; // Internet Domain
    server.sin_port = htons(8000); // Port
    server.sin_addr.s_addr = INADDR_ANY; // IP Address

    bind(s, (struct sockaddr *)&server, sizeof(server));
    client_address_size = sizeof(client);  

    // Create a text file database
    FILE *databaseFile = fopen("database.txt", "a+");

    if (databaseFile == NULL)
    {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    // =================== comm start ===================
    // receive an integer from the client
    recvfrom(s, &num, sizeof(num), 0, (struct sockaddr *)&client, &client_address_size);
    num = ntohl(num);
    printf("Int received: %d\n", ntohl(num));   
    // send a reply message to the client
    strcpy(msg, "Int received");
    sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));
    // receive menu option from the client
    recvfrom(s, &userInput, sizeof(userInput), 0, (struct sockaddr *)&client, &client_address_size);
    printf("User selection: %d\n",ntohl(userInput)); 
    // send a reply message to the client
    strcpy(msg, "User selection received");
    sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

    // perform command
    while (userInput != 6) {
        
        if (ntohl(userInput) == 1) { // add student to database
        
            // receive ID from the client
            recvfrom(s, &ID, sizeof(ID), 0, (struct sockaddr *)&client, &client_address_size);
            printf("ID: %d\n",ntohl(ID));   
            strcpy(msg, "ID received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // receive FirstName from the client
            recvfrom(s, &FirstNameSize, sizeof(FirstNameSize), 0, (struct sockaddr *)&client, &client_address_size);
            recvfrom(s, FirstName, FirstNameSize, 0, (struct sockaddr *)&client, &client_address_size);
            FirstName[FirstNameSize] = '\0';
            printf("FirstName: %s\n",FirstName);   
            strcpy(msg, "FirstName received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // receive LastName from the client
            recvfrom(s, &LastNameSize, sizeof(LastNameSize), 0, (struct sockaddr *)&client, &client_address_size);
            recvfrom(s, LastName, LastNameSize, 0, (struct sockaddr *)&client, &client_address_size);
            LastName[LastNameSize] = '\0';
            printf("LastName: %s\n",LastName);   
            strcpy(msg, "LastName received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // receive Grade from the client
            recvfrom(s, &Grade, sizeof(Grade), 0, (struct sockaddr *)&client, &client_address_size);
            printf("Grade: %d\n",ntohl(Grade));   
            strcpy(msg, "Grade received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // write student into the database file
            struct Student newStudent;
            newStudent.ID = ntohl(ID);
            strncpy(newStudent.FirstName, FirstName, sizeof(newStudent.FirstName));
            strncpy(newStudent.LastName, LastName, sizeof(newStudent.LastName));
            newStudent.Grade = ntohl(Grade);
            fprintf(databaseFile, "%d %s %s %d\n", newStudent.ID, newStudent.FirstName, newStudent.LastName, newStudent.Grade);
            fflush(databaseFile);

            // send a reply message to the client
            strcpy(msg, "New student information has been added to the database");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));
        }
        else if (ntohl(userInput) == 2) // delete student based on ID
        {
            // receive ID from the client
            recvfrom(s, &ID, sizeof(ID), 0, (struct sockaddr *)&client, &client_address_size);
            printf("ID: %d\n",ntohl(ID));   
            // send a reply message to the client
            strcpy(msg, "ID received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // create a temporary file to store the updated database
            FILE *tempFile = fopen("temp_database.txt", "w");
            if (tempFile == NULL)
            {
                perror("Error opening the temporary file");
                exit(EXIT_FAILURE);
            }

            // Iterate through the database and copy all students except the one to be deleted to the temporary file
            struct Student currentStudent;
            int targetID = ntohl(ID);
            int found = 0;
            
            fseek(databaseFile, 0, SEEK_SET);
            while (fscanf(databaseFile, "%d %s %s %d", &currentStudent.ID, currentStudent.FirstName, currentStudent.LastName, &currentStudent.Grade) == 4)
            {
                if (currentStudent.ID == targetID)
                {
                    found = 1;
                    continue;
                }

                fprintf(tempFile, "%d %s %s %d\n", currentStudent.ID, currentStudent.FirstName, currentStudent.LastName, currentStudent.Grade);
            }

            fclose(tempFile);
            fclose(databaseFile);

            // Remove the original database file
            remove("database.txt");

            // Rename the temporary file to the original database file
            if (rename("temp_database.txt", "database.txt") != 0)
            {
                perror("Error renaming the temporary file");
                exit(EXIT_FAILURE);
            }

            // Open the updated database file for further use
            databaseFile = fopen("database.txt", "a+");

            if (databaseFile == NULL)
            {
                perror("Error opening the database file");
                exit(EXIT_FAILURE);
            }

            // Send a reply message to the client
            if (found)
                strcpy(msg, "Student information deleted from the database");
            else
                strcpy(msg, "Student with that ID was not found");

            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));
        }
        else if (ntohl(userInput) == 3) // display student based on ID match
        {
            // receive ID from the client
            recvfrom(s, &ID, sizeof(ID), 0, (struct sockaddr *)&client, &client_address_size);
            cID = ntohl(ID);
            printf("ID: %d\n", cID);
            strcpy(msg, "ID received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // search for the student with matching ID 
            fseek(databaseFile, 0, SEEK_SET);
            struct Student currentStudent;
            int targetID = cID;
            int found = 0;
            while (fscanf(databaseFile, "%d %s %s %d", &currentStudent.ID, currentStudent.FirstName, currentStudent.LastName, &currentStudent.Grade) == 4)
            {
                if (currentStudent.ID == targetID)
                {
                    found = 1;
                    break;
                }
            }

            // send student information or a flag indicating not found
            if (found)
            {
                sendto(s, &currentStudent, sizeof(currentStudent), 0, (struct sockaddr *)&client, sizeof(client));
            }
            else
            {
                struct Student notFoundStudent;
                notFoundStudent.ID = -1;
                sendto(s, &notFoundStudent, sizeof(notFoundStudent), 0, (struct sockaddr *)&client, sizeof(client));
            }
        }
        else if (ntohl(userInput) == 4) // display all students who have a Grade above Grade sent from client
        {
            // receive Grade from the client
            recvfrom(s, &Grade, sizeof(Grade), 0, (struct sockaddr *)&client, &client_address_size);
            printf("Grade: %d\n",ntohl(Grade));   
            strcpy(msg, "Grade received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // send information of all those students found to the client
            struct Student currentStudent;
            int targetGrade = ntohl(Grade);
            int found = 0;
            fseek(databaseFile, 0, SEEK_SET);

            while (fscanf(databaseFile, "%d %s %s %d", &currentStudent.ID, currentStudent.FirstName, currentStudent.LastName, &currentStudent.Grade) == 4)
            {
                if (currentStudent.Grade > targetGrade)
                {
                    sendto(s, &currentStudent, sizeof(currentStudent), 0, (struct sockaddr *)&client, sizeof(client));
                    found = 1;
                }
            }

            if (!found)
            {
                struct Student noDataFlag;
                noDataFlag.ID = -2;
                sendto(s, &noDataFlag, sizeof(noDataFlag), 0, (struct sockaddr *)&client, sizeof(client));
            }
            else
            {
                struct Student endFlag;
                endFlag.ID = -1;
                sendto(s, &endFlag, sizeof(endFlag), 0, (struct sockaddr *)&client, sizeof(client));
            }
        }
        else if (ntohl(userInput) == 5) // display all students in database
        {
            // send information of all the students
            struct Student currentStudent;
            fseek(databaseFile, 0, SEEK_SET);
            while (fscanf(databaseFile, "%d %s %s %d", &currentStudent.ID, currentStudent.FirstName, currentStudent.LastName, &currentStudent.Grade) == 4)
            {
                sendto(s, &currentStudent, sizeof(currentStudent), 0, (struct sockaddr *)&client, sizeof(client));
            }

            struct Student endFlag;
            endFlag.ID = -1;
            sendto(s, &endFlag, sizeof(endFlag), 0, (struct sockaddr *)&client, sizeof(client));
        }

        // receive menu option for next loop
        recvfrom(s, &userInput, sizeof(userInput), 0, (struct sockaddr *)&client, &client_address_size);
        printf("User selection: %d\n",ntohl(userInput)); 

        // send a reply message to the client
        strcpy(msg, "User selection received");
        sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));
    }

    //remove("student_database.txt");
    fclose(databaseFile);
    close(s);

    return 0;
}
