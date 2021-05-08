#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <stdbool.h>

#define TRUE   1 
#define FALSE  0 
#define PORT 8080 

int sd;

void login();
void regis();
bool checkId(FILE*, char*);
void getInput(char*, char*);


int main() {
    int server_fd, new_socket, valread, client_socket[30], max_clients = 30, activity, i;  
    int max_sd, ready = 1, running = 0;
    struct sockaddr_in address;
    int opt = TRUE;
    int addrlen = sizeof(address);

    char buffer[1028];

    fd_set readfds;

    for(i=0; i<max_clients; i++) 
        client_socket[i] = 0;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(TRUE) {
        //clear the socket set 
        FD_ZERO(&readfds);  
     
        //add master socket to set 
        FD_SET(server_fd, &readfds);  
        max_sd = server_fd;  

        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  
                 
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  

        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
             
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(server_fd, &readfds))  
        {  
            new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            if (new_socket < 0)
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }
            if(ready && client_socket[0] == 0) {
                char *welcome = "Server is ready!\nMenu : (case sensitive)\n1. 'register'\n2. 'login'\n\0";
                if(send(new_socket, welcome, strlen(welcome), 0 ) == strlen(welcome)) {
                    ready = 0;
                    puts("Welcome message sent successfully");
                }
                else {
                    perror("send");
                    puts("Welcome message failed to send");
                } 
            }
            else {
                char *welcome_delay = "Server is busy!\nPlease wait for other client to process";
                if(send(new_socket, welcome_delay, strlen(welcome_delay), 0 ) != strlen(welcome_delay)) {
                    perror("send");
                    puts("Welcome message failed to send");
                }
                else puts("Welcome message sent successfully");
            }
                    
            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                            
                    break;  
                }  
            }  
        }

        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
                 
            if (FD_ISSET( sd , &readfds))  
            {  
                char buffer[1024] = {0};
                valread = read( sd , buffer, 1024);  
                if (valread == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n" , 
                            inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0; 
                    int tempsocket = client_socket[0];
                    for(int j = i; j < max_clients-1; j++) {
                        client_socket[j] = client_socket[j+1];
                        
                    }
                    client_socket[max_clients-1] = 0;
                    ready = 1;

                    if(client_socket[0] != 0 && ready && tempsocket == 0) {
                        char *welcome = "Server is ready!\nMenu : (case sensitive)\n1. 'register'\n2. 'login'\n\0";
                        send(client_socket[0], welcome, strlen(welcome), 0 );
                        ready = 0;
                    } else {
                        ready = 1;
                    }
                }   
                else {
                    if(!ready && sd == client_socket[0]) {
                        if(strcmp(buffer, "login") == 0) {
                            login();
                        }
                        else if(strcmp(buffer, "register") == 0) {
                            regis();
                        } 
                        else {
                            char *invalid = "invalid command\0";
                            send(sd , invalid, strlen(invalid) , 0 );
                        }
                    }
                    else {
                        char *welcome_delay = "Server is busy!\nPlease wait for other client to process\0";
                        if(send(sd, welcome_delay, strlen(welcome_delay), 0 ) != strlen(welcome_delay)) {
                            perror("send");
                            puts("Message failed to send");
                }
                    }
                }
            }
        }
    }
    return 0;
}

void getInput(char* username, char* password) {
    char unm[1024] = "enter username\0";
    send(sd , unm, strlen(unm) , 0 );
    read(sd, username, 1024);
    char pswd[1024] = "enter password\0";
    send(sd , pswd, strlen(pswd) , 0 );
    read(sd, password, 1024);
}

void login() {
    char username[1024] = {};
    char password[1024] = {};
    FILE *src = fopen("akun.txt", "a+");
    getInput(username, password);
    char comb[2049] = {};
    sprintf(comb, "%s:%s\n", username, password);
    char file[1024] = {};
    int info = 0;
    if(checkId(src, username)) {
        FILE *src = fopen("akun.txt", "a+");
        while(fgets(file, 1024, src) != NULL) {
            printf("%s - %s",file,comb);
            if(strcmp(file, comb) == 0) {
                char *success = "Login success\n";
                send(sd , success, strlen(success) , 0 );
                info = 1;
                
            }
        }
        if(info == 0) {
            char *fail = "Wrong password";
            send(sd , fail, strlen(fail) , 0 );
        }
        printf("%d\n",info);
    }
    else {
        char *notExist = "ID not found\n";
        send(sd , notExist, strlen(notExist) , 0 );
    }
    fclose(src);
}

void regis() {
    char username[1024] = {};
    char password[1024] = {};
    FILE *src = fopen("akun.txt", "a+");
    getInput(username, password);
    char file[1024] = {};
    if(checkId(src, username)) {
        char *fail = "Account is already registered\n";
        send(sd , fail, strlen(fail) , 0 );
    } else {
        FILE *src = fopen("akun.txt", "a+");
        char cat[1024];
        strcpy(cat, username);
        strcat(cat, ":");
        strcat(cat, password);
        fprintf(src, "%s:%s\n", username, password);
        // fprintf(src, cat, NULL);
        char *success = "Register success\n";
        send(sd , success, strlen(success) , 0 );
    }
    fclose(src);
}

bool checkId(FILE *src, char* id) {
    char file[1024] = {};
    while(fscanf(src, "%s", file) != EOF) {
        char *tmp = strtok(file, ":");
        if(!strcmp(id, tmp)) {
            fclose(src);
            return true;
        }
    }
    fclose(src);
    return false;
}