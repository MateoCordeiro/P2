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
    char Fname[10];
    char Lname[10];
    int score;
};

int main(int argc, char **argv){

    int s,
        namelen,
        client_address_size,
        userInput,
        ID, 
        cID,
        score, 
        cscore;
    
    struct sockaddr_in server, 
        client;

    char buffer[2048],
        msg[1024],
        Fname[10],
        Lname[10];

    uint32_t num, 
        FnameSize, 
        LnameSize;

    /* Create a datagram socket in the internet domain and use the UDP protocol. */
    s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET; /* Server is in Internet Domain */
    server.sin_port = htons(8000); /* port */
    server.sin_addr.s_addr = INADDR_ANY; /* Server's Internet Address */

    bind(s, (struct sockaddr *)&server, sizeof(server));
    client_address_size = sizeof(client);  

    // Create a text file to serve as student database
    FILE *databaseFile = fopen("student_database.txt", "a+");

    if (databaseFile == NULL)
    {
        perror("Error opening the database file");
        exit(EXIT_FAILURE);
    }

    // communication starts from here

    // receive an integer from the client
    recvfrom(s, &num, sizeof(num), 0, (struct sockaddr *)&client, &client_address_size);
    num = ntohl(num);
    printf("Integer received: %d\n", ntohl(num));   

    // send a reply message to the client
    strcpy(msg, "Integer received");
    sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

    // receive menu option from the client
    recvfrom(s, &userInput, sizeof(userInput), 0, (struct sockaddr *)&client, &client_address_size);
    printf("User selection: %d\n",ntohl(userInput)); 

    // send a reply message to the client
    strcpy(msg, "User selection received");
    sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

    // perform command
    while (userInput != 6)
    {
        if (ntohl(userInput) == 1) // add student to database
        {
            // receive ID from the client
            recvfrom(s, &ID, sizeof(ID), 0, (struct sockaddr *)&client, &client_address_size);
            printf("ID: %d\n",ntohl(ID));   
            strcpy(msg, "ID received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // receive Fname from the client
            recvfrom(s, &FnameSize, sizeof(FnameSize), 0, (struct sockaddr *)&client, &client_address_size);
            recvfrom(s, Fname, FnameSize, 0, (struct sockaddr *)&client, &client_address_size);
            Fname[FnameSize] = '\0';
            printf("Fname: %s\n",Fname);   
            strcpy(msg, "Fname received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // receive Lname from the client
            recvfrom(s, &LnameSize, sizeof(LnameSize), 0, (struct sockaddr *)&client, &client_address_size);
            recvfrom(s, Lname, LnameSize, 0, (struct sockaddr *)&client, &client_address_size);
            Lname[LnameSize] = '\0';
            printf("Lname: %s\n",Lname);   
            strcpy(msg, "Lname received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // receive score from the client
            recvfrom(s, &score, sizeof(score), 0, (struct sockaddr *)&client, &client_address_size);
            printf("score: %d\n",ntohl(score));   
            strcpy(msg, "score received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // write student into the database file
            struct Student newStudent;
            newStudent.ID = ntohl(ID);
            strncpy(newStudent.Fname, Fname, sizeof(newStudent.Fname));
            strncpy(newStudent.Lname, Lname, sizeof(newStudent.Lname));
            newStudent.score = ntohl(score);
            fprintf(databaseFile, "%d %s %s %d\n", newStudent.ID, newStudent.Fname, newStudent.Lname, newStudent.score);
            fflush(databaseFile);

            // send a reply message to the client
            strcpy(msg, "New student information has been added to the database");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));
        }
        else if (ntohl(userInput) == 2) // display student based on ID match
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
            while (fscanf(databaseFile, "%d %s %s %d", &currentStudent.ID, currentStudent.Fname, currentStudent.Lname, &currentStudent.score) == 4)
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
        else if (ntohl(userInput) == 3) // display all students who have a score above score sent from client
        {
            // receive score from the client
            recvfrom(s, &score, sizeof(score), 0, (struct sockaddr *)&client, &client_address_size);
            printf("score: %d\n",ntohl(score));   
            strcpy(msg, "score received");
            sendto(s, msg, sizeof(msg), 0, (struct sockaddr *)&client, sizeof(client));

            // send information of all those students found to the client
            struct Student currentStudent;
            int targetScore = ntohl(score);
            int found = 0;
            fseek(databaseFile, 0, SEEK_SET);

            while (fscanf(databaseFile, "%d %s %s %d", &currentStudent.ID, currentStudent.Fname, currentStudent.Lname, &currentStudent.score) == 4)
            {
                if (currentStudent.score > targetScore)
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
        else if (ntohl(userInput) == 4) // display all students in database
        {
            // send information of all the students
            struct Student currentStudent;
            fseek(databaseFile, 0, SEEK_SET);
            while (fscanf(databaseFile, "%d %s %s %d", &currentStudent.ID, currentStudent.Fname, currentStudent.Lname, &currentStudent.score) == 4)
            {
                sendto(s, &currentStudent, sizeof(currentStudent), 0, (struct sockaddr *)&client, sizeof(client));
            }

            struct Student endFlag;
            endFlag.ID = -1;
            sendto(s, &endFlag, sizeof(endFlag), 0, (struct sockaddr *)&client, sizeof(client));
        }
        else if (ntohl(userInput) == 5) // delete student based on ID
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
            while (fscanf(databaseFile, "%d %s %s %d", &currentStudent.ID, currentStudent.Fname, currentStudent.Lname, &currentStudent.score) == 4)
            {
                if (currentStudent.ID == targetID)
                {
                    found = 1;
                    continue;
                }

                fprintf(tempFile, "%d %s %s %d\n", currentStudent.ID, currentStudent.Fname, currentStudent.Lname, currentStudent.score);
            }

            fclose(tempFile);
            fclose(databaseFile);

            // Remove the original database file
            remove("student_database.txt");

            // Rename the temporary file to the original database file
            if (rename("temp_database.txt", "student_database.txt") != 0)
            {
                perror("Error renaming the temporary file");
                exit(EXIT_FAILURE);
            }

            // Open the updated database file for further use
            databaseFile = fopen("student_database.txt", "a+");

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
