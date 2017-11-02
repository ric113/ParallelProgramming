#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>


void* localTossing(void* arg) {
    
    long long int toss = (long long int)arg;
    long long int count;
    long long int *inCircleCount = (long long int*)malloc(sizeof(long long int));
    *inCircleCount = 0 ;
    double random_x, random_y, distanceSquared;
    pthread_t threadId = pthread_self();
    unsigned int seed = (unsigned long int)threadId;

    for(count = 0 ; count < toss ; count ++) {
        random_x = (double)rand_r(&seed)/RAND_MAX*2.0-1.0;
        random_y = (double)rand_r(&seed)/RAND_MAX*2.0-1.0;
        distanceSquared = random_x * random_x + random_y * random_y;

        if(distanceSquared <= 1)
            (*inCircleCount)++;       
    }
    
    return inCircleCount;
}

int main(int argc, char *argv[]) {

    srand(time(NULL));


    long long int threadCount, tossCount;
    
    threadCount = strtoll(argv[1], NULL, 10);
    tossCount = strtoll(argv[2], NULL, 10);

    pthread_t* threadHandles;

    threadHandles = (pthread_t*)malloc(threadCount * sizeof(pthread_t));

    long long int count ;
    long long int restToss = tossCount % threadCount;
    long long int tossPerThread;
    for(count = 0 ; count < threadCount ; count ++ ){

        if( count < restToss )
            tossPerThread = tossCount / threadCount + 1;
        else
            tossPerThread = tossCount / threadCount;

        pthread_create(&threadHandles[count], NULL, localTossing, (void*) tossPerThread);
    }

    long long int *returnValue ;
    long long int totalSuccessToss = 0 ;

    for(count = 0 ; count < threadCount ; count ++) {
        pthread_join(threadHandles[count],(void**)&returnValue);
        // printf("Thread %lld local successed toss sum is %lld\n",count,*returnValue);
        totalSuccessToss += *returnValue;
    }

    // printf("total: %lld\n", totalSuccessToss);

    double pi = 4 * totalSuccessToss / (double)tossCount;

    printf("pi = %f\n", pi);
    
    
    free(threadHandles);
    
    return 0;
}