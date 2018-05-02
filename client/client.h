/*
    tKDCClient
    CS 445G - Project 2
    Trevor D. Brown and Travis Anderson

    Created by Trevor D. Brown on 4/28/18.
    Copyright © 2018 Trevor D. Brown. All rights reserved.

    client.h - the header of the client application.
*/

#ifndef client_h
#define client_h

// Preprocessor macros
#define KDC_ID 100;     // KDC Server ID

// Included modules
#include <stdio.h>      // Standard I/O
#include <stdlib.h>     // Standard Library
#include <unistd.h>     // POSIX OS API
#include <string.h>     // String
#include <time.h>       // Time

// Message Passing (part of POSIX OS API)
#include <sys/ipc.h>    // Interprocess Communication (IPC)
#include <sys/types.h>  // Types
#include <sys/msg.h>    // Messaging

// Structure for client application
struct client{
    int id;
    key_t sharedKey;
    key_t sessionKey;
} client;

// Stucture for sending message to KDC server application with the source and destination IDs for client direct communcation.
struct idMessage {
    long mType;
    int sID;
    int dID;
} message;

// Structure for receiving message from KDC server containing the session key.
struct sessionMessage{
    long mType;
    int sessionKey;
} sessionMessage;

// Structure for sending and receiving general messages to and from clients.
struct generalMessage{
    long mType;
    int fromID;
    int messageContent;
} generalMessage;

// int main(int argc, const char * argv[]);
// the entrypoint for the application
int main(int argc, const char * argv[]);

// void sendIDs(void)
// sends a message to the KDC server with information regarding the source and desintations for a session
// No parameters, no returns.
void sendIDs(void);

// void receiveSessionKey(void);
// receives a session key from the KDC server.
// No parameters, no returns.
void receiveSessionKey(void);

// void sendMessage(void);
// Sends a message to a friend in an active session.
// No parameters, no returns.
void sendMessage(void);

// void receiveMessage(void);
// Receives a message from a friend in an active session.
// No parameters, no returns.
void receiveMessage(void);

// int encryptDecryptMessage(int sharedKey, int message);
// Encrypts and decrypts messages/keys based on the client's shared key, or session key.
int encryptDecryptMessage(int sharedKey, int message);

#endif /* client_h */
