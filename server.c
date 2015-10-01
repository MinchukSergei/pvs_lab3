#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int main(int argc, char **argv)
{
    int sock, listener;       // дескрипторы сокетов
    struct sockaddr_in addr; // структура с адресом
    char buf[2048];       // буфур для приема
    int bytes_read;           // кол-во принятых байт
    int portno; 
    FILE *command_file; 
    int nread = 0;
    
    if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
 
    listener = socket(AF_INET, SOCK_STREAM, 0); // создаем сокет для входных подключений
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    portno = atoi(argv[1]);
    // Указываем параметры сервера
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(portno);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) // связываемся с сетевым устройство. Сейчас это устройство lo - "петля", которое используется для отладки сетевых приложений
    {
        perror("bind");
        exit(2);
    }
 
    listen(listener, 5); // очередь входных подключений
    
    while(1)
    {
        sock = accept(listener, NULL, NULL); // принимаем входные подключение и создаем отделный сокет для каждого нового подключившегося клиента
        if(sock < 0)
        {
            perror("Прием входящих подключений");
            exit(3);
        }
 
    switch(fork())
    {
    case -1:
        perror("fork");
        break;

    case 0:
        close(listener);
        while(1)
        {
            printf("\nОжидаем сообщение...\n");
            memset(buf, 0, sizeof(buf));
            nread = 0;
            bytes_read = recv(sock, buf, sizeof(buf), 0); // принимаем сообщение от клиента
            if(bytes_read <= 0)
                break;
            printf("Получено %d bytes\tСообщение: %s", bytes_read, buf);
            command_file = popen(buf, "r");
            
            while((nread = fread(buf, 1, sizeof(buf), command_file)) > 0)
            {
                fwrite(buf, sizeof(char), nread, stdout);
                send(sock, buf, nread, 0); // отправляем принятое сообщение клиенту
            }
            memset(buf, 0, sizeof(buf));
            send(sock, buf, sizeof(buf), 0); // отправляем принятое сообщение клиенту
            pclose(command_file);
        }
        
        close(sock); // закрываем сокет
        exit(0);

    default:
        close(sock);
    }   
}
   
return 0;
}