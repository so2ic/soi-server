#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

typedef struct 
{
    int connfd;
} thread_args;

#endif // SERVER_HPP
