//
//  main.h
//  tKDCClient
//
//  Created by Trevor D. Brown on 4/28/18.
//  Copyright © 2018 Trevor D. Brown. All rights reserved.
//

#ifndef main_h
#define main_h

// KDC ID
#define KDC_ID 100;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// Message Passing
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

struct {
    int id;
    key_t sharedKey;
    key_t sessionKey;
} client;

struct idMessage {
    long mType;
    int sID;
    int dID;
} message;

struct sessionMessage{
    long mType;
    int sessionKey;
} sessionMessage;

struct generalMessage{
    long mType;
    int fromID;
    int messageContent;
} generalMessage;

// Main Function
int main(int argc, const char * argv[]);

// Generate ID
int generateID(void);

// Set Shared Key
// K x,KDC
int setSharedKey(void);

// Send IDs
void sendIDs(void);

// Receive Session Key
void receiveSessionKey(void);

// Send Message
void sendMessage(void);

// Receive Message
void receiveMessage(void);

// Decrypt Key
int encryptDecryptMessage(int sharedKey, int message);

#endif /* main_h */
