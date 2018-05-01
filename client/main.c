//
//  main.c
//  tKDCClient
//
//  Created by Trevor D. Brown on 4/28/18.
//  Copyright © 2018 Trevor D. Brown. All rights reserved.
//

#include "main.h"

int main(int argc, const char * argv[]) {
    
//    key_t key = 100;
//    int kdcMailQueue = msgget(key, 0600 | IPC_CREAT);
//    perror("test");
//
//    if(msgctl(kdcMailQueue, IPC_RMID, NULL) < 0){
//        perror("msgctl");
//        exit(3);
//    }
//    return 0;
    
    char userCommand[255];
    client.id = -1;
    
    while ((client.id > 3) || (client.id < 0)){
        // Set shared key for client
        printf("Possible client configurations:\n");
        printf("Client ID: 0, Shared Key: 24\n");
        printf("Client ID: 1, Shared Key: 26\n");
        printf("Client ID: 2, Shared Key: 28\n");
        printf("Client ID: 3, Shared Key: 30\n");
        printf("Enter your client ID: ");
        scanf("%i", &client.id);
        
        if (client.id == 0){
            client.sharedKey = 24;
        }
        else if (client.id == 1){
            client.sharedKey = 26;
        }
        else if (client.id == 2){
            client.sharedKey = 28;
        }
        else if (client.id == 3){
            client.sharedKey = 30;
        }
    }

    // Show menu
    strcpy(userCommand, "a");
    
    printf("\n\nIf you are the initial client, please run (SendIDs).\n");
    
    while (strcmp(userCommand, "Exit") != 0){
        // Menu Title
        printf("\n\nOptions:\n");
        
        // Send IDs
        printf("(SendIDs) to the KDC\n");
        
        // Receive Session Key
        printf("(ReceiveSessionKey) from KDC\n");
        
        // Send message
        printf("(SendMessage) to friend\n");
        
        // Receive message
        printf("(ReceiveMessage) from a friend\n");
        
        // Exit
        printf("(Exit) the program\n");
        
        scanf("%s", userCommand);
        
        if (strcmp(userCommand, "SendIDs") == 0){
            if (client.sessionKey != 0){
                printf("Warning: sending a new IDs will clear your current session. Do you want to continue, (Yes) or (No)?");
                char response[3];
                scanf("%s", response);
                if (strcmp(response, "Yes") == 0){
                    sendIDs();
                }
            }
            else{
                sendIDs();
            }
            
        }
        else if (strcmp(userCommand, "ReceiveSessionKey") == 0){
            receiveSessionKey();
        }
        else if (strcmp(userCommand, "SendMessage") == 0){
            if (client.sessionKey <= 0){
                printf("\n\nError: no active session. Please run (ReceiveSessionKey) first.\n");
            }
            else{
                sendMessage();
            }
        }
        else if (strcmp(userCommand, "ReceiveMessage") == 0){
            if (client.sessionKey <= 0){
                printf("\n\nError: no active session. Please run (ReceiveSessionKey) first.\n");
            }
            else{
                receiveMessage();
            }
        }
        else if (strcmp(userCommand, "Exit") == 0){
            
        }
        else{
            printf("Error: invalid command.\n");
        }
        
    }
    
    return 0;
}

// Send IDs
void sendIDs(){
    int destination = -1;
    
    key_t key = 100;
    
    while ((destination == client.id) || (destination < 0) || (destination > 3)){
        printf("Who do you want to send the message to (client 0 - 3, excluding yourself)?");
        scanf("%i", &destination);
    }
    
    int kdcMailQueue = msgget(key, 0600 | IPC_CREAT);
    
    // Prepare message to send.
    message.mType = 1;
    message.sID = client.id;
    message.dID = destination;
    
    // Determine length of message
    int length = sizeof(message) - sizeof(long);

    // Send message to KDC with client ID and destination ID
    long result = msgsnd(kdcMailQueue, &message, length, 0);
    
    if (result == -1){
        printf("Error: message not sent!\n");
    }
    else{
        printf("Message sent!\n");
    }
    
    result = msgrcv(kdcMailQueue, &sessionMessage, length, 1, 0);
    printf("Received Encrypted Session Key: %i\n", sessionMessage.sessionKey);
    client.sessionKey = encryptDecryptMessage(client.sharedKey, sessionMessage.sessionKey);
    printf("Decrypted Session Key: %i\n", client.sessionKey);
    
    sendMessage();
    
}

void receiveSessionKey(){
    int key = client.sharedKey;
    int kdcMailQueue = msgget(key, 0600 | IPC_CREAT);
    
    // Determine length of message
    int length = sizeof(sessionMessage) - sizeof(long);
    
    printf("Waiting for session key...\n");
    long result = msgrcv(kdcMailQueue, &sessionMessage, length, 1, 0);
    
    if (result == -1){
        printf("Error: Message failed to be received.\n");
    }
    else{
        printf("Received Encrypted Session Key: %i\n", sessionMessage.sessionKey);
        client.sessionKey = encryptDecryptMessage(client.sharedKey, sessionMessage.sessionKey);
        printf("Decrypted Session Key: %i\n", client.sessionKey);
        
        receiveMessage();
    }
}

void sendMessage(){
    int messageContent = 0;
    printf("What message would you like to send?\n");
    scanf("%i", &messageContent);
    
    int encryptedMessage = encryptDecryptMessage(client.sessionKey, messageContent);
    int friendSendQueue = msgget(client.sessionKey, 0600 | IPC_CREAT);
    int friendReceiveQueue = msgget(client.sessionKey + 1, 0600 | IPC_CREAT);
    generalMessage.mType = 1;
    generalMessage.fromID = client.id;
    generalMessage.messageContent = encryptedMessage;
    int length = sizeof(generalMessage) - sizeof(long);
    long result = msgsnd(friendSendQueue, &generalMessage, length, 0);
    
    if (result == -1){
        printf("Error: message not sent!\n");
    }
    else{
        printf("Message sent!\n");
        
        // Determine length of message
        length = sizeof(generalMessage) - sizeof(long);
        
        printf("Wating for response...\n");
        result = msgrcv(friendReceiveQueue, &generalMessage, length, 1, 0);
        
        if (result == -1){
            printf("Error: Message failed to be received.\n");
        }
        else{
            int messageFromFriend = encryptDecryptMessage(client.sessionKey, generalMessage.messageContent);
            printf("Message receipt confirmed!\n");
            printf("Message from client %i: %i\n", generalMessage.fromID, messageFromFriend);
        }
    }
}

void receiveMessage(){
    int friendReceiveQueue = msgget(client.sessionKey, 0600 | IPC_CREAT);
    int friendSendQueue = msgget(client.sessionKey + 1, 0600 | IPC_CREAT);
    
    // Determine length of message
    int length = sizeof(generalMessage) - sizeof(long);
    
    printf("Waiting for message...\n");
    long result = msgrcv(friendReceiveQueue, &generalMessage, length, 1, 0);
    
    if (result == -1){
        printf("Error: Message failed to be received.\n");
    }
    else{
        int messageFromFriend = encryptDecryptMessage(client.sessionKey, generalMessage.messageContent);
        printf("Message from client %i: %i\n", generalMessage.fromID, messageFromFriend);
        
        int messageContent = 0;
        printf("What response would you like to send?\n");
        scanf("%i", &messageContent);
        
        int encryptedMessage = encryptDecryptMessage(client.sessionKey, messageContent);
        generalMessage.mType = 1;
        generalMessage.fromID = client.id;
        generalMessage.messageContent = encryptedMessage;
        length = sizeof(generalMessage) - sizeof(long);
        result = msgsnd(friendSendQueue, &generalMessage, length, 0);
        
        if (result == -1){
            printf("Error: message not sent!\n");
        }
        else{
            printf("Response sent!\n");
        }
    }
}


int encryptDecryptMessage(int sharedKey, int message){
    int result = sharedKey ^ message;
    return result;
}
