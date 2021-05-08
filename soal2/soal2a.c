#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define r1 4
#define c1 3
#define r2 3
#define c2 6

int matrix1[r1][c1];
int matrix2[r2][c2];

void *multiplier(void *argv) {
    int *result = (int*)argv;
    int x = 0;
    int y = 0;

    for(x = 1; x <= c1; x++)
        y += result[x]*result[x+c1];
    
    int *p = (int*)malloc(sizeof(int));
    *p = y;
    pthread_exit(p);
}

int main() {

    int i, j, k;

    printf("Matriks 1 (4 x 3) :\n");
    for (i = 0; i < r1; i++)
        for (j = 0; j < c1; j++)
            scanf("%d", &matrix1[i][j]);

    printf("Matriks 2 (3 x 6) :\n");
    for (i = 0; i < r2; i++)
        for (j = 0; j < c2; j++)
            scanf("%d", &matrix2[i][j]);

    pthread_t *threadsid;
    threadsid = (pthread_t*)malloc((24)*sizeof(pthread_t));

    int count = 0;
    int *result = NULL;

    for(i = 0; i < r1; i++) {
        for(j = 0; j < c2; j++) {
            result = (int *)malloc((24)*sizeof(int));
            result[0] = c1;

            for(k = 0; k < c1; k++) {
                result[k+1] = matrix1[i][k];
            }

            for(k = 0; k < c1; k++) {
                result[k+1+r1] = matrix2[i][k];
            }

            pthread_create(&(threadsid[count++]), NULL, &multiplier,(void*) result);
        }
    }

    printf("\nHasil :\n");
    for (i = 0; i < r1*c2; i++) {
        void *k;
        pthread_join(threadsid[i], &k);
        int *p = (int *)k;

        printf("%d ",*p);
        if((i + 1) % c2 == 0) printf("\n");
    }

    return 0;
}
