/*
    tKDCServer
    CS 445G - Project 2
    Trevor D. Brown and Travis Anderson

    Created by Trevor D. Brown on 4/28/18.
    Copyright © 2018 Trevor D. Brown. All rights reserved.

    server.c - the implementation of the server application.
*/

#include "server.h" // the header "companion" file for this file.

// int main(int argc, const char * argv[])
// The entry point for the application.
int main(int argc, const char * argv[]) {

    // Clears out any instances of the kdcMailQueue from prior runs on the server.
    key_t key = 100;
    int kdcMailQueue = msgget(key, 0600 | IPC_CREAT);

    if(msgctl(kdcMailQueue, IPC_RMID, NULL) < 0){
        perror("msgctl");
        exit(3);
    }
    // End clear procedure

    // Inform the user that the server has started.
    printf("Starting Key Distribution Center...\n");

    // Stores predefined keys for the four client instances.
    int sharedKeyCounter = 24;
    for (int i = 0; i < sizeof(server.sharedKeys); i++){
        server.sharedKeys[i] = sharedKeyCounter;
        sharedKeyCounter += 2;
    }

    // Create KDC Message (Mail) queue
    key = 100;
    kdcMailQueue = msgget(key, 0600 | IPC_CREAT);

    // Determine the length of the message
    int length = sizeof(idMessage) - sizeof(long);

    // Prepare a way to receive a message from a client.
    long result;
    // Listen for the message from the client
    result = msgrcv(kdcMailQueue, &idMessage, length, 1, 0);

    // If a message is "received", but cannot be interpreted...
    if (result == -1){
        printf("Error: Message failed to be received.\n");
        return 0;
    }
    else{
        // Otherwise, process the message.
        printf("ID Message: From %i to %i \n", idMessage.sID, idMessage.dID);

        // Store the source and destination keys from the message.
        server.sourceSharedKey = server.sharedKeys[idMessage.sID];
        server.destinationSharedKey = server.sharedKeys[idMessage.dID];

        printf("Source Shared Key: %i\n", server.sourceSharedKey);
        printf("Destination Shared Key: %i\n", server.destinationSharedKey);

        // Generate the session key.
        int sessionKey = generateSessionKey();

        // Encrypt session key with shared keys from hosts.
        int encryptedSessionKeySource = encryptKey(server.sourceSharedKey, sessionKey);
        int encryptedSessionKeyDestination = encryptKey(server.destinationSharedKey, sessionKey);

        // Send encrypted session key to the source.
        sessionMessage.mType = 1;
        sessionMessage.sessionKey = encryptedSessionKeySource;
        length = sizeof(sessionMessage) - sizeof(long);
        result = msgsnd(kdcMailQueue, &sessionMessage, length, 0);

        // Send encrypted session key to the destination.
        key_t destinationKey = server.destinationSharedKey;
        int destinationMailQueue = msgget(destinationKey, 0600 | IPC_CREAT);
        sessionMessage.mType = 1;
        sessionMessage.sessionKey = encryptedSessionKeyDestination;
        length = sizeof(sessionMessage) - sizeof(long);
        result = msgsnd(destinationMailQueue, &sessionMessage, length, 0);

        if(msgctl(kdcMailQueue, IPC_RMID, NULL) < 0){
            perror("msgctl");
            exit(3);
        }
    }

    return 0;
}

// generateSessionKey()
// Uses random number generator to make a "raw" session key.
int generateSessionKey(){
    srand(time(NULL));
    int sessionKey = rand() % RAND_MAX;
    printf("Session Key is %i\n", sessionKey);
    return sessionKey;
}

// encryptKey(int sharedKey, int message)
// encrypts a message (or another key) with a shared key.
int encryptKey(int sharedKey, int message){
    int result = sharedKey ^ message;
    printf("Result of Encrypt function is: %i\n", result);
    return result;
}
