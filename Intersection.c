#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h> 
#include "Intersection.h"

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__)
#else
#define VERBOSE_PRINT(S, ...) ((void) 0) // do nothing
#endif

#define MAX_OCCUPANCY          3
#define NUM_ITERATIONS         100
#define NUM_CARS               20
#define FAIR_WAITING_COUNT     4

// These times determine the number of times yield is called when in
// the street, or when waiting before crossing again.
#define CROSSING_TIME             20
#define WAIT_TIME_BETWEEN_CROSSES 10

enum Direction {NORTH = 1, SOUTH = -1, EAST = 2, WEST = -2};
const static enum Direction oppositeEndNS [] = {SOUTH, NORTH};
const static enum Direction oppositeEndEW [] = {EAST, WEST};

STRUCT Street street;

typedef struct Street
{
    pthread_mutex_t mutex;
    pthread_mutex_t north;
    pthread_mutex_t south;
    pthread_mutex_t east;
    pthread_mutex_t west;
    int num_cars;
    int cars_in;
    enum Direction dir;
} Street;

void initializeStreet (void)
{
    street.mutex = PTHREAD_MUTEX_INITIALIZER();
    street.north = pthread_mutex_init(street.mutex);
    street.south = pthread_mutex_init(street.mutex);
    street.east = pthread_mutex_init(street.mutex);
    street.west = pthread_mutex_init(street.mutex);
    street.num_cars = 0;
    street.cars_in = 0;
    street.dir = 1;
}

#define WAITING_HISTOGRAM_SIZE (NUM_ITERATIONS * NUM_CARS)
int             entryTicker;                                          // incremented with each entry
int             waitingHistogram [WAITING_HISTOGRAM_SIZE];
int             waitingHistogramOverflow;
pthread_mutex_t waitingHistogramMutex;
int             occupancyHistogram [2] [MAX_OCCUPANCY + 1];

void enterStreet (enum Direction d)
{

}

void leaveStreet (void)
{

}

void recordWaitingTime (int waitingTime)
{

}

void* cars ()
{

}

int main (int argc, char** argv)
{
    
}
