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

// Struct for student information
struct Student {
    int id;
    char firstName[10];
    char lastName[10];
    int grade;
};

int main(int argc, char *argv[]) {
    int sock, menuChoice, studentID, studentScore;
    
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientSize = sizeof(clientAddr);  // Correct data type for client address length
    
    char recvBuffer[2048], 
         sendBuffer[1024], 
         tempFirstName[10], 
         tempLastName[10];

    uint32_t receivedInt, fnameLength, lnameLength;

    // Create a UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serverAddr.sin_family = AF_INET;  // Internet Domain
    serverAddr.sin_port = htons(8000);  // Server port
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Any IP address

    if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(sock);
        return -1;
    }

    // Create or open the student database file
    FILE *studentDB = fopen("students_db.txt", "a+");
    if (!studentDB) {
        perror("Failed to open student database");
        close(sock);
        return -1;
    }

    // Start communication with the client
    recvfrom(sock, &receivedInt, sizeof(receivedInt), 0, (struct sockaddr *)&clientAddr, &clientSize);
    receivedInt = ntohl(receivedInt);  // Convert to host byte order
    printf("Received integer: %d\n", receivedInt);

    strcpy(sendBuffer, "Integer received");
    sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, &clientSize);

    recvfrom(sock, &menuChoice, sizeof(menuChoice), 0, (struct sockaddr *)&clientAddr, &clientSize);
    printf("User menu choice: %d\n", ntohl(menuChoice));

    strcpy(sendBuffer, "Menu choice received");
    sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, &clientSize);

    while (ntohl(menuChoice) != 6) {  // 6 is the exit option
        switch (ntohl(menuChoice)) {
            case 1:  // Add student to the database
                // Receive student ID from the client
                recvfrom(sock, &studentID, sizeof(studentID), 0, (struct sockaddr *)&clientAddr, &clientSize);
                studentID = ntohl(studentID);
                printf("Student ID: %d\n", studentID);

                strcpy(sendBuffer, "ID received");
                sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, &clientSize);

                // Receive the student's first name
                recvfrom(sock, &fnameLength, sizeof(fnameLength), 0, (struct sockaddr *)&clientAddr, &clientSize);
                recvfrom(sock, tempFirstName, fnameLength, 0, (struct sockaddr *)&clientAddr, &clientSize);
                tempFirstName[fnameLength] = '\0';

                printf("First name: %s\n", tempFirstName);

                strcpy(sendBuffer, "First name received");
                sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, &clientSize);

                // Receive the student's last name
                recvfrom(sock, &lnameLength, sizeof(lnameLength), 0, (struct sockaddr *)&clientAddr, &clientSize);
                recvfrom(sock, tempLastName, lnameLength, 0, (struct sockaddr *)&clientAddr, &clientSize);
                tempLastName[lnameLength] = '\0';

                printf("Last name: %s\n", tempLastName);

                strcpy(sendBuffer, "Last name received");
                sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, &clientSize);

                // Receive the student's score
                recvfrom(sock, &studentScore, sizeof(studentScore), 0, (struct sockaddr *)&clientAddr, &clientSize);
                studentScore = ntohl(studentScore);

                printf("Score: %d\n", studentScore);

                strcpy(sendBuffer, "Score received");
                sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, &clientSize);

                // Save the student information in the database
                struct Student newStudent;
                newStudent.id = studentID;
                strncpy(newStudent.firstName, tempFirstName, sizeof(newStudent.firstName));
                strncpy(newStudent.lastName, tempLastName, sizeof(newStudent.lastName));
                newStudent.grade = studentScore;

                fprintf(studentDB, "%d %s %s %d\n", newStudent.id, newStudent.firstName, newStudent.lastName, newStudent.grade);
                fflush(studentDB);

                strcpy(sendBuffer, "Student added to the database");
                sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, clientSize);
                break;

            case 2:  // Display student by ID
                // Receive the student ID to be searched
                recvfrom(sock, &studentID, sizeof(studentID), 0, (struct sockaddr *)&clientAddr, &clientSize);
                studentID = ntohl(studentID);

                printf("Received student ID: %d\n", studentID);

                strcpy(sendBuffer, "ID received");
                sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, &clientSize);

                // Search the student database for the given ID
                fseek(studentDB, 0, SEEK_SET);
                struct Student retrievedStudent;
                int studentFound = 0;

                while (fscanf(studentDB, "%d %s %s %d", &retrievedStudent.id, retrievedStudent.firstName, retrievedStudent.lastName, &retrievedStudent.grade) == 4) {
                    if (retrievedStudent.id == studentID) {
                        studentFound = 1;
                        break;
                    }
                }

                if (studentFound) {
                    sendto(sock, &retrievedStudent, sizeof(retrievedStudent), 0, (struct sockaddr *)&clientAddr, &clientSize);
                } else {
                    struct Student notFoundStudent;
                    notFoundStudent.id = -1;
                    sendto(sock, &notFoundStudent, sizeof(notFoundStudent), 0, (struct sockaddr *)&clientAddr, clientSize);
                }
                break;

            case 3:  // Display students with scores above a certain value
                recvfrom(sock, &studentScore, sizeof(studentScore), 0, (struct sockaddr *)&clientAddr, &clientSize);
                studentScore = ntohl(studentScore);

                printf("Received score threshold: %d\n", studentScore);

                strcpy(sendBuffer, "Score received");
                sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, &clientSize);

                int anyMatches = 0;
                struct Student currentStudent;

                fseek(studentDB, 0, SEEK_SET);

                while (fscanf(studentDB, "%d %s %s %d", &currentStudent.id, currentStudent.firstName, currentStudent lastName, &currentStudent.grade) == 4) {
                    if (currentStudent.grade > studentScore) {
                        sendto(sock, &currentStudent, sizeof(currentStudent), 0, (struct sockaddr *)&clientAddr, clientSize);
                        anyMatches = 1;
                    }
                }

                if (!anyMatches) {
                    struct Student noMatch;
                    noMatch.id = -2;
                    sendto(sock, &noMatch, sizeof(noMatch), 0, (struct sockaddr *)&clientAddr, clientSize);
                }

                break;

            case 4:  // Display all students
                fseek(studentDB, 0, SEEK_SET);
                while (fscanf(studentDB, "%d %s %s %d", &currentStudent.id, currentStudent.firstName, currentStudent.lastName, &currentStudent.grade) == 4) {
                    sendto(sock, &currentStudent, sizeof(currentStudent), 0, (struct sockaddr *)&clientAddr, clientSize);
                }

                struct Student endFlag;
                endFlag.id = -1;
                sendto(sock, &endFlag, sizeof(endFlag), 0, (struct sockaddr *)&clientAddr, clientSize);
                break;

            case 5:  // Delete student based on ID
                recvfrom(sock, &studentID, sizeof(studentID), 0, (struct sockaddr *)&clientAddr, clientSize);
                studentID = ntohl(studentID);

                printf("Received ID to delete: %d\n", studentID);

                strcpy(sendBuffer, "ID received");
                sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, clientSize);

                // Temporary file for database update
                FILE *tempDB = fopen("temp_db.txt", "w");
                if (!tempDB) {
                    perror("Could not open temp database");
                    close(sock);
                    return -1;
                }

                fseek(studentDB, 0, SEEK_SET);

                int found = 0;
                while (fscanf(studentDB, "%d %s %s %d", &currentStudent.id, currentStudent.firstName, currentStudent.lastName, &currentStudent.grade) == 4) {
                    if (currentStudent.id == studentID) {
                        found = 1;
                        continue;  // Don't copy the deleted student
                    }
                    fprintf(tempDB, "%d %s %s %d\n", currentStudent.id, currentStudent.firstName, currentStudent lastName, currentStudent.grade);
                }

                fclose(tempDB);
                fclose(studentDB);

                // Replace the old database with the updated one
                remove("students_db.txt");
                if (rename("temp_db.txt", "students_db.txt") != 0) {
                    perror("Could not rename temp database");
                    close(sock);
                    return -1;
                }

                studentDB = fopen("students_db.txt", "a+");
                if (!studentDB) {
                    perror("Failed to reopen student database");
                    close(sock);
                    return -1;
                }

                if (found) {
                    strcpy(sendBuffer, "Student deleted from the database");
                } else {
                    strcpy(sendBuffer, "Student with given ID not found");
                }

                sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, clientSize);
                break;
        }

        recvfrom(sock, &menuChoice, sizeof(menuChoice), 0, (struct sockaddr *)&clientAddr, &clientSize);
        printf("User's next menu choice: %d\n", ntohl(menuChoice));

        strcpy(sendBuffer, "Next menu choice received");
        sendto(sock, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *)&clientAddr, &clientSize);
    }

    fclose(studentDB);
    close(sock);

    return 0;
}
