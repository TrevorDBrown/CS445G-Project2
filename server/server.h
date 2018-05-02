/*
    tKDCServer
    CS 445G - Project 2
    Trevor D. Brown and Travis Anderson

    Created by Trevor D. Brown on 4/28/18.
    Copyright © 2018 Trevor D. Brown. All rights reserved.

    server.h - the header of the server application.
*/

#ifndef server_h
#define server_h

// Preprocessor macros
#define KDC_ID 100  // KDC ID

// Include modules
#include <stdio.h>      // Standard I/O
#include <stdlib.h>     // Standard Library
#include <unistd.h>     // POSIX OS API
#include <string.h>     // Strings
#include <time.h>       // Time
#include <errno.h>      // Error Handling

// Message Passing modules (from POSIX OS API)
#include <sys/ipc.h>    // Interprocess Communication (IPC) 
#include <sys/types.h>  // Types
#include <sys/msg.h>    // Messaging

// Structure used for the server application
struct kdcServer{
    int sharedKeys[4];
    int sourceSharedKey;
    int destinationSharedKey;
} server;

// Structure used when receiving a message from clients regarding a source and destination ID
struct idMessage{
    long mType;
    int sID;
    int dID;
} idMessage;

// Structure used to send clients information regarding their session.
struct sessionMessage{
    long mType;
    int sessionKey;
} sessionMessage;

// int main(int argc, const char * argv[]);
// entrypoint for the program.
int main(int argc, const char * argv[]);

// int generateSessionKey(void);
// generates a session key using a RNG. 
// No parameters. Returns a random int.
int generateSessionKey(void);

// int encryptKey(int sharedKey, int sessionKey);
// generates an encrypted message or key using a client's shared key.
// requires a shared key, and a message/key. Returns an encrypted integer.
int encryptKey(int sharedKey, int sessionKey);

#endif /* server_h */
