#ifndef WEB_CLIENT_H
#define WEB_CLIENT_H

/**
\file       web_client.h
\brief      Web Client method file.

This file provides functions to handle the web client.
*/

/// Maximum character size of retrieved message string
#define MESSAGE_BUFFER_SIZE  2000

/// @brief This method configures and initialize the Web client, opens a socket from the local device to a remote web server, port 10000.
/// @param ip The IP address of the web server 
void initWebClient(char* ip);

/// @brief This method configures and initialize the Web client, opens a socket from the local device to a remote web server, port as argument.
/// @param ip The IP address of the web server 
/// @param newport The server port to use
void initWebClient_new_port(char* ip, unsigned short newport);


/// @brief This method retrieves the local IP address of a given network inferface (e.g 'wlan0')
/// @param interface The name of the local interface 
/// @return IP address of local interface
char* getMyIP (char* interface);

/// @brief This method sends a plain text message to a pre-configured web server. All messages are sent to port 10000.
/// @param mess The text message to send to the web server 
void sendMessage(char *mess);

/// @brief This method retreives a message sent by the web server (from port 10000)
/// @return The message retrieved 
const char* getMessage(void);

/// @brief Closes the socket connection to the web server
void stopWebClient(void);

#endif