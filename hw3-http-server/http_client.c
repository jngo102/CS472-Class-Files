
#include "http_client.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFF_SZ 1024
static char recv_buffer[BUFF_SZ];
static int data_socket;
char *host;
uint16_t port;

static void init_params(int argc, char **argv)
{
    if (argc < 3)
    {
        perror("Usage: ./http_client <host> <port> [request] [request] ...");
        exit(EXIT_FAILURE);
    }   

    host = argv[1];
    port = (uint16_t)atoi(argv[2]);
}

static void start_client()
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(struct sockaddr_in));

    struct hostent *hp = gethostbyname(host);
    if (hp == NULL)
    {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }
    bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    data_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (data_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    connect(data_socket, (struct sockaddr *)&addr, sizeof(addr));
}

char *generate_header_for_request(char* path)
{
    static char req[BUFF_SZ] = {0};
    int offset = 0;
    offset += sprintf(req + offset, "GET %s HTTP/1.1\r\n", path);
    offset += sprintf((char *)(req + offset), "Host: %s\r\n", host);
    offset += sprintf((char *)(req + offset), "Connection: Keep-Alive\r\n");
    offset += sprintf((char *)(req + offset), "\r\n");

    printf("\nRequest:\n%s\n", req);
    return req;
}

void send_and_recv_data(char *data)
{
    int msg_len = send(data_socket, data, strlen(data), 0);
    if (msg_len == -1)
    {
        perror("send");
        start_client();
        send_and_recv_data(data);
    }

    msg_len = recv(data_socket, recv_buffer, BUFF_SZ, 0);
    if (msg_len == -1)
    {
        perror("recv");
        start_client();
        send_and_recv_data(data);
    }

    printf("Response from server:\n%s\n", recv_buffer);
    memset(recv_buffer, 0, BUFF_SZ);
}

int main(int argc, char **argv)
{
    init_params(argc, argv);
    start_client();
    for (int i = 3; i < argc; i++)
    {
        char *req = generate_header_for_request(argv[i]);
        send_and_recv_data(req);
        sleep(1);
    }

    close(data_socket);

    return EXIT_SUCCESS;
}