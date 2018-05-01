//
//  main.h
//  tKDCServer
//
//  Created by Trevor D. Brown on 4/28/18.
//  Copyright © 2018 Trevor D. Brown. All rights reserved.
//

#ifndef main_h
#define main_h

// KDC ID
#define KDC_ID 100

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// Message Passing
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

// Error Handling
#include <errno.h>

struct kdcServer{
    int sharedKeys[4];
    int sourceSharedKey;
    int destinationSharedKey;
} server;

struct idMessage{
    long mType;
    int sID;
    int dID;
} idMessage;

struct sessionMessage{
    long mType;
    int sessionKey;
} sessionMessage;

int main(int argc, const char * argv[]);

int generateSessionKey(void);
int encryptKey(int sharedKey, int sessionKey);


#endif /* main_h */
