#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctype.h>


pthread_t tid[100000];

char *cwd;
char *tempcwd;

//mendapatkan nama file
void 
char* filename(char string[])
{
    char* cekk;
    char* hasil;
    cekk = strchr(string,'/');
    if(cekk == NULL)
    return string;
    
    while (cekk != NULL) 
    {
        hasil = cekk+1;
        cekk = strchr(cekk+1,'/');
    }
    return hasil;
}


int main(int argc, char **argv)
{
}
