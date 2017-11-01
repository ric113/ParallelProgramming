#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <pthread.h>
#include <vector>

using namespace std;


struct thread_arg {
  int id;
  long long toss;
};

vector<thread_arg> args;
vector<long long> in_circle;

void* localTossing(void* args) {
    srand(time(NULL));
    unsigned seed = rand();
    for (long long i = 0; i < ((thread_arg *)args)->toss; ++i) {
    double x = double(rand_r(&seed)) / RAND_MAX;
    double y = double(rand_r(&seed)) / RAND_MAX;
    if (x * x + y * y <= 1) {
      in_circle[((thread_arg *)args)->id]++;
    }
  }
    
    return NULL;
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
    in_circle = vector<long long>(threadCount, 0);
    for(count = 0 ; count < threadCount ; ++count ){

        if( count < restToss )
            tossPerThread = tossCount / threadCount + 1;
        else
            tossPerThread = tossCount / threadCount;

        struct thread_arg t;
        t.id = count;
        t.toss = tossPerThread;
        args.push_back(t);

        pthread_create(&threadHandles[count], NULL, localTossing, &args[count]);
    }

    long long int *returnValue ;
    long long int totalSuccessToss = 0 ;

    for(count = 0 ; count < threadCount ; ++count) {
        cout << "Yo" << endl;
        pthread_join(threadHandles[count],NULL);
    }

    for (int i = 0; i < threadCount; ++i) {
      totalSuccessToss += in_circle[i];
  }

    printf("total: %lld\n", totalSuccessToss);

    double pi = 4 * totalSuccessToss / (double)tossCount;

    printf("pi = %f\n", pi);
    pthread_exit(NULL);
    
    free(threadHandles);
    
    return 0;
}