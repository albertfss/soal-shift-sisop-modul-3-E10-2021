#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <pthread.h>


pthread_t thread[1000];
int many =1;
char *perintah;
int keberhasilan = 1;

void *myFile (void *judulFiles) 
{
    char *ext;
    char judulBaru[2000];
    char *judulFiles1 = (char*) judulFiles;
    snprintf(judulBaru, sizeof judulBaru, "%s", judulFiles1);
    char *judul = judulFiles;
    char *namaFiles;
    namaFiles = strrchr(judulFiles, '/');
    namaFiles = strtok(namaFiles, "/");
    char namaFilesLama[10000];
    snprintf(namaFilesLama, sizeof namaFilesLama, "%s", namaFiles);
    if(namaFiles[0]=='.')
    {
        ext = "hidden";
    }
    else
    {
        namaFiles = strtok(namaFiles, ".");
        if(strcmp(namaFilesLama, namaFiles)==0)
        {
            ext = "unknown"; 
        }
        else
        {
            ext = strtok(NULL, "");
            for (int i=0;i < strlen(ext);i++)
            {
                ext[i] = tolower(ext[i]);
            }
        }
    }
    mkdir (ext,0777);
    char pindah[20000];
    snprintf(pindah, sizeof pindah, "%s/%s", ext, namaFilesLama);
    int hasil;
    hasil = rename(judulBaru, pindah);
    if(strcmp(perintah, "-f")==0)
    {
        if(hasil == 0) 
        {
            printf("File %d : Berhasil Dikategorikan\n",many);
        }
        else
        {
            printf("File %d : Sad,gagal :(\n",many);
        }
        many++;
    }
    else
    {
        if(hasil != 0)
        {
            keberhasilan = 0;
        }
    }
}

char namaFilesNih[10000][10000];
int nomor = 0;
void recursive(char *basePath)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        { 
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            strcpy(namaFilesNih[nomor], path);
            nomor++;
            recursive(path);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (strcmp(argv[1],"-f")==0)
    {  
        perintah = argv[1];
        int jumlah = 2;
        int jalankan = 2;
        while (jumlah < argc) 
        {
            pthread_create(&(thread[jumlah]), NULL, myFile, (char*)argv[jumlah]);
            jumlah++;
        }
        while (jalankan < argc) 
        {
            pthread_join(thread[jalankan],NULL);
            jalankan++;
        }
    }

    else if (strcmp(argv[1],"-d") == 0 ) 
    { 
        perintah = argv[1];
        recursive(argv[2]);
        for(int i=0; i<nomor; i++)
        {
            pthread_create(&(thread[i]), NULL, myFile, (char*)namaFilesNih[i]);
        }
        for(int i=0; i<nomor; i++)
        {
            pthread_join(thread[i],NULL);
        }
        if(keberhasilan == 1)
        {
            printf("Direktori sukses disimpan!\n");
        }else
        {
            printf("Yah, gagal disimpan :(\n");
        }
    }

    else if (strcmp(argv[1],"*") == 0 ) 
    { 
        perintah = argv[1];
        recursive(".");
        for(int i=0; i<nomor; i++)
        {
            pthread_create(&(thread[i]), NULL, myFile, (char*)namaFilesNih[i]);
        }
        for(int i=0; i<nomor; i++)
        {
            pthread_join(thread[i],NULL);
        }
        if(keberhasilan == 1)
        {
            printf("Yah, gagal disimpan :(\n");
        }
        else
        {
            printf("Direktori sukses disimpan!\n");
        }
    }
    return 0;
}
