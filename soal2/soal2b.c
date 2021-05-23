#include<stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>

typedef long long int ll;

int *shared_matrix, *matrix_b;
ll *result;

typedef struct pair{
    int a;
    int b;
    int index;
}pair;

void *factorial(void *y){
    pair *temp = (pair *) y;
    ll temp_result = 1, i;
    if(temp->a == 0 || temp->b == 0){
        result[temp->index] = 0;
    }
    else if(temp->a >= temp->b){
        for (i = temp->a - temp->b + 1; i <= temp->a; i++) temp_result *= i;
        result[temp->index] = temp_result;
    }
    else if(temp->a < temp->b) {
        for (i = 1; i <= temp->a; i++) temp_result *= i;
        result[temp->index] = temp_result;
    }
}

int main() {
    key_t key = 1945;

    int shmid = shmget(key, sizeof(int)*24, IPC_CREAT | 0666);
    shared_matrix = shmat(shmid, NULL, 0);

    matrix_b = (int*) malloc(24 * sizeof(int));
    result = (ll*) malloc(24 * sizeof(ll));
    
    for(int i=0;i<24;i++) {
        scanf("%d",&matrix_b[i]);
    }

    pthread_t threadsid[24];

    for(int i=0;i<24;i++){
        pair *tes = (pair*)malloc(sizeof(*tes));
        tes->a = shared_matrix[i];
        tes->b = matrix_b[i];
        tes->index = i;
        if(pthread_create(&threadsid[i],NULL,factorial,(void *)tes)!=0){
            fprintf(stderr, "error: Cannot create thread # %d\n",i);
        }
    }

    for (int i = 0; i < 24; ++i){
      if (pthread_join(threadsid[i], NULL))
        {
          fprintf(stderr, "error: Cannot join thread # %d\n", i);
        }
    }

    for(int i=0;i<24;i++){
        if(i%6==0)
            printf("\n");
        printf("%lld\t",result[i]);
    }

    printf("\n");
    shmdt(shared_matrix);
    shmctl(shmid, IPC_RMID, NULL);
}
