#include <stdio.h>
#include <string.h>    //strlen, strcmp
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <sys/types.h>
#include <ifaddrs.h>

#include "web_client.h"

static char myIP[NI_MAXHOST] = "";
unsigned short port = 10000;
struct sockaddr_in server;
int socket_desc;
int connected;
char serverIp[20];

char* getMyIP (char* interface)
{
    struct ifaddrs *ifaddr, *ifa;
    int s;

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;
        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),myIP, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if((strcmp(ifa->ifa_name,interface)==0)&&(ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            break;
        }
    }
    freeifaddrs(ifaddr);
    return myIP;
}


void connectToServer(void)
{
    if(!connected)
    {
        //Create socket
        printf("Create socket\n");
        socket_desc = socket(AF_INET, SOCK_STREAM , 0);
        if (socket_desc < 0)
        {
            fprintf(stderr, "connect error");
            return;
        }
        printf("Configure socket\n");
        server.sin_addr.s_addr = inet_addr(serverIp);
        server.sin_family = AF_INET;
        server.sin_port = htons( port );
        //Connect to remote server
        printf("Connect to remote server\n");
        if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
        {
            fprintf(stderr, "connect error\n");
            return;
        }
        printf("Connected to server %s:%d\n", serverIp, port);
        fflush(stdout);
        connected = 1;
    }
}


void initWebClient(char* ip)
{
    strncpy(serverIp, ip,  sizeof serverIp - 1);
    connected = 0;
    connectToServer();
}

void initWebClient_new_port(char* ip, unsigned short newport)
{
    strncpy(serverIp, ip,  sizeof serverIp - 1);
    connected = 0;
    port = newport;
    connectToServer();
}


void sendMessage(char *mess)
{
    if(connected)
    {
        if( send(socket_desc , mess , strlen(mess) , 0) < 0)
        {
            perror("send");
            connected = 0;
        }
    }
}


const char* getMessage(void)
{
    char *server_reply = (char*)malloc(MESSAGE_BUFFER_SIZE * sizeof(char));
    server_reply[0] = '\0';
    if(connected)
    {
        if( recv(socket_desc, server_reply , MESSAGE_BUFFER_SIZE , 0) < 0)
        {
            perror("recv");
            connected = 0;
        }
    }
    return server_reply;
}


void stopWebClient(void)
{
    if(connected)
    {
        if( close(socket_desc) < 0)
        {
            perror("close");
            connected = 0;
        }
    }
}
