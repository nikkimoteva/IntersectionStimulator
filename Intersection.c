#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> 
//#include <semaphore.h> use dispatch for OS X
#include <dispatch/dispatch.h>
#include <sched.h>
#include "Intersection.h"

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__)
#else
#define VERBOSE_PRINT(S, ...) ((void) 0) // do nothing
#endif

#define MAX_OCCUPANCY          3
#define NUM_ITERATIONS         100
#define NUM_CARS               20
//#define FAIR_WAITING_COUNT     4
#define TIMEOUT                300

// These times determine the number of times yield is called when in
// the street, or when waiting before crossing again.
// #define CROSSING_TIME             20
// #define WAIT_TIME_BETWEEN_CROSSES 10

//const static enum Direction oppositeEndEW [] = {WEST, EAST};
//sem_t semMut;
struct Street
{
    dispatch_semaphore_t mutex;
    dispatch_semaphore_t hasSpace;
    dispatch_semaphore_t hasCars;
    dispatch_semaphore_t dirlock;
    int numCars;
    int direction;
} Street;

struct Street street;

void initializeStreet (void)
{
    street.mutex = dispatch_semaphore_create(0);
    street.hasSpace = dispatch_semaphore_create(0);
    street.hasCars = dispatch_semaphore_create(0);
    street.dirlock = dispatch_semaphore_create(0);
    street.numCars = 0;
    street.direction = NORTH;
    printf("done 1");
}

void destroyStreet (void)
{
    dispatch_release(street.mutex);
    dispatch_release(street.hasSpace);
    dispatch_release(street.hasCars);
    dispatch_release(street.dirlock);
}


#define WAITING_HISTOGRAM_SIZE (NUM_ITERATIONS * NUM_CARS)
int                  entryTicker;                                 // incremented with each entry
int                  waitingHistogram [WAITING_HISTOGRAM_SIZE];
int                  waitingHistogramOverflow;
dispatch_semaphore_t waitingHistogramMutex;
int                  occupancyHistogram [2] [MAX_OCCUPANCY + 1];

void enterStreet (enum Direction d)
{
    dispatch_semaphore_wait(street.hasSpace, TIMEOUT);
    if (d != street.direction){
        dispatch_semaphore_wait(street.dirlock, TIMEOUT);
        street.direction = d;
    }
    dispatch_semaphore_wait(street.mutex, TIMEOUT);
    dispatch_semaphore_wait(waitingHistogramMutex, TIMEOUT);
    entryTicker++;
    dispatch_semaphore_signal(street.hasCars);
    dispatch_semaphore_signal(waitingHistogramMutex);
    dispatch_semaphore_signal(street.hasSpace);
    printf("done 2");
}

void leaveStreet (void)
{
    dispatch_semaphore_wait(street.hasCars, TIMEOUT);
    dispatch_semaphore_wait(street.mutex, TIMEOUT);
    street.numCars--;
    if (street.numCars == 0){
        // street.direction = oppositeEnd[street.direction];
        // sem_post(&street.direction);
        dispatch_semaphore_signal(street.dirlock);
        dispatch_semaphore_signal(street.dirlock);
        dispatch_semaphore_signal(street.dirlock);
    }
    dispatch_semaphore_signal(street.hasSpace);
    dispatch_semaphore_signal(street.mutex);
    printf("done 3");
}

void waitingTime (int waitingTime)
{
    dispatch_semaphore_wait(waitingHistogramMutex, TIMEOUT);
    if (waitingTime < WAITING_HISTOGRAM_SIZE)
        waitingHistogram[waitingTime] ++;
    else waitingHistogramOverflow ++;
    dispatch_semaphore_signal(waitingHistogramMutex);
}

void* cars (void* dir)
{
    printf("in 0");
    int* d = dir;
    for (int i = 0; i < NUM_ITERATIONS; i++){
		int begin = entryTicker;
		enterStreet(*d);
		int end = entryTicker;
		waitingTime(end - begin);
        for (int j = 0; j < NUM_CARS; j++){
            sched_yield();
        }
        leaveStreet();
        for (int k = 0; k < NUM_CARS; k ++){
            sched_yield();
        }
    }
    printf("done 0");
    return NULL;
}

int main (int argc, char** argv)
{
    //pthread_attr_init(NUM_CARS); 
    printf("here ahhhh0");
    initializeStreet();
    pthread_t pt[NUM_CARS];
    printf("here ahhhh");

    //init
    //pthread_t *arrayPtr = malloc(sizeof(pthread_t) * NUM_CARS);
    int* rando = malloc(sizeof(int));
    for (int i = 0; i < NUM_CARS; i++){
        *rando = rand() % 4;
        //memcpy(rando,rand() % 4,sizeof(rando));
        pthread_create(&pt[i], NULL, cars, &rando); //.........
        //arrayPtr[i] = car;
    }
    for (int i = 0; i < NUM_CARS; i++) {
        pthread_join(pt[i], 0);
    }

    printf ("Times with 1 car  going north: %d\n", occupancyHistogram [NORTH] [1]);
    printf ("Times with 2 cars going north: %d\n", occupancyHistogram [NORTH] [2]);
    printf ("Times with 3 cars going north: %d\n", occupancyHistogram [NORTH] [3]);
    printf ("Times with 1 car  going south: %d\n", occupancyHistogram [SOUTH] [1]);
    printf ("Times with 2 cars going south: %d\n", occupancyHistogram [SOUTH] [2]);
    printf ("Times with 3 cars going south: %d\n", occupancyHistogram [SOUTH] [3]);
    printf ("Times with 1 car  going east: %d\n",  occupancyHistogram [EAST]  [1]);
    printf ("Times with 2 cars going east: %d\n",  occupancyHistogram [EAST]  [2]);
    printf ("Times with 3 cars going east: %d\n",  occupancyHistogram [EAST]  [3]);
    printf ("Times with 1 car  going west: %d\n",  occupancyHistogram [WEST]  [1]);
    printf ("Times with 2 cars going west: %d\n",  occupancyHistogram [WEST]  [2]);
    printf ("Times with 3 cars going west: %d\n",  occupancyHistogram [WEST]  [3]);
  
    printf ("Waiting Histogram\n");
    for (int i=0; i<WAITING_HISTOGRAM_SIZE; i++)
        if (waitingHistogram [i])
        printf ("Cars waited for %4d car%s to enter: %4d time(s)\n",
	    i, i==1 ? " " : "s", waitingHistogram [i]);
    if (waitingHistogramOverflow)
        printf ("Cars waited for more than %4d cars to enter: %4d time(s)\n",
	            WAITING_HISTOGRAM_SIZE, waitingHistogramOverflow);

    
    destroyStreet();
}
