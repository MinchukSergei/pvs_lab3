#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    char message[2048];//=(char*)malloc(sizeof(char));// сообщение на передачу
    char buf[sizeof(message)];
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    while(1) {
    memset(message, 0, sizeof(message));
    memset(buf, 0, sizeof(buf));
    printf("Введите сообщение серверу(Для выхода:ctrl+C): "); 
    fgets(message, sizeof(message), stdin);
 
    send(sockfd, message, sizeof(message), 0); // отправка сообщения на сервер
    int bytes_read = 0;
        while (1) {
            bytes_read = recv(sockfd, buf, sizeof(buf), 0);
            if (strlen(buf) == 0)
            {
                break;
            }
            fwrite(buf, sizeof(char), bytes_read, stdout);// прием сообщения от сервера
        }
 
    }
    return 0;
}