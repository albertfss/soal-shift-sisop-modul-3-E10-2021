#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

#define PORT 8080
#define DATA_BUFFER 1024

const int SIZE_BUFFER = sizeof(char) * DATA_BUFFER;
char inputPath[DATA_BUFFER];
bool _inputPath = false;

int create_tcp_client_socket();
void *handleInput(void *client_fd);
void *handleOutput(void *client_fd);
void getServerInput(int fd, char *input);
void sendFile(int fd);
void writeFile(int fd);

int main()
{
    pthread_t tid[2];
    int client_fd = create_tcp_client_socket();

    pthread_create(&(tid[0]), NULL, &handleOutput, (void *) &client_fd);
    pthread_create(&(tid[1]), NULL, &handleInput, (void *) &client_fd);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    close(client_fd);
    return 0;
}

int create_tcp_client_socket()
{
    struct sockaddr_in serv_addr;
    int sock;
    int opt = 1;
    struct hostent *local_host; 

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
    return sock;
}

void *handleInput(void *client_fd)
{
    int fd = *(int *) client_fd;
    char message[DATA_BUFFER] = {0};

    while (1) {
        // fgets(message, DATA_BUFFER, stdin);
        gets(message);
        send(fd, message, SIZE_BUFFER, 0);
        if (_inputPath) {
            strcpy(inputPath, message);
        }
    }
}

void *handleOutput(void *client_fd) 
{
    int fd = *(int *) client_fd;
    char message[DATA_BUFFER] = {0};

    while (1) {
        memset(message, 0, SIZE_BUFFER);
        // getServerInput(fd, message);
        if (read(fd, message, DATA_BUFFER) == 0) {
            printf("Server shutdown\n");
            exit(EXIT_SUCCESS);
        }
        printf("%s", message);
        
        if (strcmp(message, "Filepath: ") == 0) {
            _inputPath = true;
        } else if (strcmp(message, "Start sending file\n") == 0) {
            sendFile(fd);
            _inputPath = false;
        } else if (strcmp(message, "Error: file is already uploaded\n") == 0) {
            _inputPath = false;
        } else if (strcmp(message, "Start receiving file\n") == 0) {
            writeFile(fd);
        } 
        fflush(stdout);
    }
}

void sendFile(int fd)
{
    printf("Sending [%s] file to server\n", inputPath);
    int valread;
    FILE *fp = fopen(inputPath, "r");
    char buf[DATA_BUFFER] = {0};

    if (fp) {
        send(fd, "File found", SIZE_BUFFER, 0);

        fseek(fp, 0L, SEEK_END);
        int size = ftell(fp);
        rewind(fp);
        sprintf(buf, "%d", size);
        send(fd, buf, SIZE_BUFFER, 0);

        while ((valread = fread(buf, 1, 1, fp)) > 0) {
            send(fd, buf, 1, 0);
        } 
        printf("Sending file finished\n");
        fclose(fp);
    } else {
        printf("File not found\n");
        send(fd, "File not found", SIZE_BUFFER, 0);
    }
}

void writeFile(int fd)
{
    char buff[DATA_BUFFER] = {0};
    int valread = read(fd, buff, DATA_BUFFER);
    FILE *fp = fopen(buff, "w+");

    read(fd, buff, DATA_BUFFER);
    int size = atoi(buff);
    
    while (size > 0) {
        valread = read(fd, buff, DATA_BUFFER);
        fwrite(buff, 1, valread, fp);
        memset(buff, 0, SIZE_BUFFER);
        size -= valread;
    }
    puts("Send file finished");
    send(fd, "Send file finished", SIZE_BUFFER, 0);
    fclose(fp);
}

void getServerInput(int fd, char *input)
{
    if (read(fd, input, DATA_BUFFER) == 0) {
        printf("Server shutdown\n");
        exit(EXIT_SUCCESS);
    }
}