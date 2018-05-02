/*
    tKDCClient
    CS 445G - Project 2
    Trevor D. Brown and Travis Anderson

    Created by Trevor D. Brown on 4/28/18.
    Copyright © 2018 Trevor D. Brown. All rights reserved.

    client.c - the implementation of the client application.
*/

#include "client.h" // the header "companion" file for this file.

// int main(int argc, const char * argv[])
// The entry point for the application.
int main(int argc, const char * argv[]) {
    
    // Declare variables to be used throughout the function.
    char userCommand[255];
    client.id = -1;
    
    // Ask the user to enter a valid client ID (0 - 3)
    // Ask again if out of range.
    while ((client.id > 3) || (client.id < 0)){
        // Set shared key for client
        printf("Possible client configurations:\n");
        printf("Client ID: 0, Shared Key: 24\n");
        printf("Client ID: 1, Shared Key: 26\n");
        printf("Client ID: 2, Shared Key: 28\n");
        printf("Client ID: 3, Shared Key: 30\n");
        printf("Enter your client ID: ");
        scanf("%i", &client.id);
        
        // If a valid client ID, store the appropriate shared key.
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

    // Show user command menu
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
        
        // If the client enters "SendIDs"...
        if (strcmp(userCommand, "SendIDs") == 0){
            // If the client's session ID is already populated, warn the user their current session will be cleared.
            if (client.sessionKey != 0){
                printf("Warning: sending a new IDs will clear your current session. Do you want to continue, (Yes) or (No)?");
                char response[3];
                scanf("%s", response);
                if (strcmp(response, "Yes") == 0){
                    sendIDs();
                }
            }
            else{
                // Otherwise, call the sendIDs() function
                sendIDs();
            }
            
        }
        // If the client enters "ReceiveSessionKey"...
        else if (strcmp(userCommand, "ReceiveSessionKey") == 0){
            // Call the receiveSessionKey() command.
            receiveSessionKey();
        }
        // If the client enters "SendMessage"...
        else if (strcmp(userCommand, "SendMessage") == 0){
            // Validate a session is active.
            // If not, throw an error. Otherwise, call sendMessage() function.
            if (client.sessionKey <= 0){
                printf("\n\nError: no active session. Please run (ReceiveSessionKey) first.\n");
            }
            else{
                sendMessage();
            }
        }
        // If the client enters "ReceiveMessage"...
        else if (strcmp(userCommand, "ReceiveMessage") == 0){
            // Validate a session is active.
            // If not, throw an error. Otherwise, call receiveMessage() function.
            if (client.sessionKey <= 0){
                printf("\n\nError: no active session. Please run (ReceiveSessionKey) first.\n");
            }
            else{
                receiveMessage();
            }
        }
        // If the client enters "Exit"
        else if (strcmp(userCommand, "Exit") == 0){
            // Do nothing...
            // This is to prevent "else" from taking over.
        }
        else{
            printf("Error: invalid command.\n");
        }
        
    }
    
    // Once "Exit" is entered, end the program.
    return 0;
}

// SendIDs()
// sends the client (source) and friend's (destination) IDs to the KDC server application via IPC.
void sendIDs(){
    // Declare the destination variable.
    int destination = -1;
    
    // Declare the KDC server message (mail) queue ID.
    key_t key = 100;
    
    // Ask the user to enter a valid friend ID.
    // Verify the input is not the user's ID or out of range.
    // Ask again if it does not succeed.
    while ((destination == client.id) || (destination < 0) || (destination > 3)){
        printf("Who do you want to send the message to (client 0 - 3, excluding yourself)?");
        scanf("%i", &destination);
    }
    
    // Establish the KDC server message (mail) queue, for communication.
    int kdcMailQueue = msgget(key, 0600 | IPC_CREAT);
    
    // Prepare message to send.
    message.mType = 1;
    message.sID = client.id;
    message.dID = destination;
    
    // Determine length of message
    int length = sizeof(message) - sizeof(long);

    // Send message to KDC with client ID and destination ID
    long result = msgsnd(kdcMailQueue, &message, length, 0);
    
    // If the message fails to be added to the message queue, throw an error.
    // Otherwise, inform the user the message has been sent.
    if (result == -1){
        printf("Error: message not sent!\n");
    }
    else{
        printf("Message sent!\n");
    }
    
    // Wait for a message to be received from the KDC server, regarding the session key.
    result = msgrcv(kdcMailQueue, &sessionMessage, length, 1, 0);
    printf("Received Encrypted Session Key: %i\n", sessionMessage.sessionKey);
    client.sessionKey = encryptDecryptMessage(client.sharedKey, sessionMessage.sessionKey);
    printf("Decrypted Session Key: %i\n", client.sessionKey);
    
    // When the session key has been received, call the sendMessage() function.
    sendMessage();
    
}

// receiveSessionKey()
// receives an encrypted copy of the session key from the KDC server.
void receiveSessionKey(){
    // Store client's shared key as the key for the KDC message (mail) queue.
    int key = client.sharedKey;

    // Establish the KDC message (mail) queue.
    int kdcMailQueue = msgget(key, 0600 | IPC_CREAT);
    
    // Determine length of message
    int length = sizeof(sessionMessage) - sizeof(long);
    
    // Wait for the KDC server to return an ecrypted copy of the session key.
    printf("Waiting for session key...\n");
    long result = msgrcv(kdcMailQueue, &sessionMessage, length, 1, 0);
    
    // If the message fails to be received, throw an error.
    // Otherwise, decrypt the session key, and store it in the client structure.
    if (result == -1){
        printf("Error: Message failed to be received.\n");
    }
    else{
        printf("Received Encrypted Session Key: %i\n", sessionMessage.sessionKey);
        client.sessionKey = encryptDecryptMessage(client.sharedKey, sessionMessage.sessionKey);
        printf("Decrypted Session Key: %i\n", client.sessionKey);
        
        // Call receive message, and wait for the friend to send a message.
        receiveMessage();
    }
}

// sendMessage()
// allows a client to send a message (int) to their friend.
void sendMessage(){
    // Declare variable for storing the message content.
    int messageContent = 0;

    // Ask the user to enter a message (int).
    printf("What message would you like to send?\n");
    scanf("%i", &messageContent);
    
    // Encrypt the message.
    int encryptedMessage = encryptDecryptMessage(client.sessionKey, messageContent);

    // Create two queues - one to receive messages from the friend, and one to send messages to the client.
    // NOTE: this was done as per a limitation/issue with a single queue using IPC.
    // Erroneously, but accurately, the sender would receive their own message immediately upon adding it to the queue.
    int friendSendQueue = msgget(client.sessionKey, 0600 | IPC_CREAT);
    int friendReceiveQueue = msgget(client.sessionKey + 1, 0600 | IPC_CREAT);

    // Prepare the message to send.
    generalMessage.mType = 1;
    generalMessage.fromID = client.id;
    generalMessage.messageContent = encryptedMessage;

    // Determine the length of the message
    int length = sizeof(generalMessage) - sizeof(long);

    // Send the message to the friend.
    long result = msgsnd(friendSendQueue, &generalMessage, length, 0);
    
    // If the message fails to send, throw an error.
    // Otherwise, tell the user the message sent, and wait for a reply.
    if (result == -1){
        printf("Error: message not sent!\n");
    }
    else{
        printf("Message sent!\n");
        
        // Determine length of message
        length = sizeof(generalMessage) - sizeof(long);
        
        // Wait for a message response from the client.
        printf("Wating for response...\n");
        result = msgrcv(friendReceiveQueue, &generalMessage, length, 1, 0);
        
        // If a message was not received, throw an error.
        // Otherwise, print the message.
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

// receiveMessage()
// allows a client to receive a message (int) from their friend.
void receiveMessage(){
    // Create two queues - one to receive messages from the friend, and one to send messages to the client.
    // NOTE: this was done as per a limitation/issue with a single queue using IPC.
    // Erroneously, but accurately, the sender would receive their own message immediately upon adding it to the queue.
    int friendReceiveQueue = msgget(client.sessionKey, 0600 | IPC_CREAT);
    int friendSendQueue = msgget(client.sessionKey + 1, 0600 | IPC_CREAT);
    
    // Determine length of message
    int length = sizeof(generalMessage) - sizeof(long);
    
    // Wait for a message from a friend with the same session key.
    printf("Waiting for message...\n");
    long result = msgrcv(friendReceiveQueue, &generalMessage, length, 1, 0);
    
    // If a message was not received, throw an error.
    // Otherwise, print the message, and prepare a new message as a reply.
    if (result == -1){
        printf("Error: Message failed to be received.\n");
    }
    else{
        int messageFromFriend = encryptDecryptMessage(client.sessionKey, generalMessage.messageContent);
        printf("Message from client %i: %i\n", generalMessage.fromID, messageFromFriend);
        
        // Ask the user to send a message back.
        int messageContent = 0;
        printf("What response would you like to send?\n");
        scanf("%i", &messageContent);
        
        // Prepare the message:
        // Encrypt the message content with a session key.
        int encryptedMessage = encryptDecryptMessage(client.sessionKey, messageContent);
        // Prepare message contents
        generalMessage.mType = 1;
        generalMessage.fromID = client.id;
        generalMessage.messageContent = encryptedMessage;

        // Determine message sise
        length = sizeof(generalMessage) - sizeof(long);
        
        // Send the message.
        result = msgsnd(friendSendQueue, &generalMessage, length, 0);
        
        // If the message send, let the user know.
        // Otherwise, throw an error.
        if (result == -1){
            printf("Error: message not sent!\n");
        }
        else{
            printf("Response sent!\n");
        }
    }
}

// encruptDecryptMessage(int sharedKey, int message)
// encrypts or decrypts a message/key using the client's shared key.
int encryptDecryptMessage(int sharedKey, int message){
    int result = sharedKey ^ message;
    return result;
}
