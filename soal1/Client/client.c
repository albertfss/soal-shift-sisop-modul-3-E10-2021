#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define PORT 8080

int sock = 0, valread;
int flag=0;

void *input(void *arg);
void *output(void *arg);

int main(int argc, char const *argv[]) {
    pthread_t thread[2];
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1) {
        pthread_create(&thread[0], NULL, output, NULL);
        pthread_create(&thread[1], NULL, input, NULL);
        pthread_join(thread[0], NULL);
        pthread_join(thread[1], NULL);
        
    }
    return 0;
}

void *output(void *arg) {
    char buffer[1024] = {};
    valread = read(sock, buffer, 1024);
    printf("%s\n",buffer);
}

void *input(void *arg) {
    char buffer[1024] = {};
    scanf("%s", buffer);
    send(sock, buffer, sizeof(buffer), 0);
}
