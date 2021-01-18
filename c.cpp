
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>
#include <iostream>

using namespace std;

#define bufSize 7
#define numSlaves 4

int buffer[bufSize][2];
pthread_mutex_t mutex;
sem_t empty, full;
int numJobs;
int duration;
int masWait;
int position;
// int job[2] = {0, 0};

void *master(void *);
void *slave(void *);

//Declare an Integer variable.
pthread_t Master;
pthread_t Slaves[numSlaves];


//Definition of the function
void initialize() {
    //Thread initializes to null
    if(pthread_mutex_init(&mutex, NULL) < 0) {
        perror("Mutex mutex could not be created");
    }
    //Keep track of number of empty slots.
    if(sem_init(&empty, 1, bufSize) < 0) {
        perror("Semaphore empty could not be created");
    }
    //Keep the track of number of full slots.
    if(sem_init(&full, 1, 0) < 0) {
        perror("Semaphore full could not be created");
    }
    //Initialize the counter
    position = 0;
}

void add(int id, int dur) {
    buffer[position][0] = id;
    buffer[position][1] = dur;
    position++;
    time_t tt;
    struct tm *ti;
    time(&tt);
    ti = localtime(&tt);
    printf("Master: created request ID %d, length %d seconds at time ", id, dur);
    cout << asctime(ti);
}

int remove() {
    //Read data at counter position
    //Return the value
    position--;
    int job = buffer[position][0];
    // job[0] = buffer[position][0];
    // job[1] = buffer[position][1];
    time_t tt;
    struct tm *ti;
    time(&tt);
    ti = localtime(&tt);
    printf("Slave: assigned request ID %d, processing request for the next %d seconds, current time is ", buffer[position][0], buffer[position][1]);
    cout << asctime(ti);;
    return job;
}

// void isEmpty() {
//     while(position > 0) {
//         wait
//     }
// }

//This function displays how producer produces item.
void *master(void *id) {
    for(int i = 1; i <= numJobs; i++) {
        int dur = (rand() % duration) + 1;
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        add(i, dur);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        printf("Master: sleeping for %d seconds\n", masWait);
        sleep(masWait);
    }
}

//Function for consumer for consuming an item

void *slave(void *id) {
    // pthread_t pthread_self(void);
    int val = true;
    while(val) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int job = remove();
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        sleep(buffer[job][1]);
        time_t tt;
        struct tm *ti;
        time(&tt);
        ti = localtime(&tt);
        printf("Slave %d: completed request ID %d at time ", (int) pthread_self(), job);
        cout << asctime(ti) << "\n";
        if(job == numJobs)
            val = false;
    }
    printf("All jobs have been completed\nGoodbye!");
    std::terminate();
}

//start the main function

int main() {
    //Call to the initialize function
    initialize();

    printf("Enter number of jobs to create: ");
    scanf("%d", &numJobs);
    
    printf("Enter the max request duration (in seconds): ");
    scanf("%d", &duration);

    printf("Enter the wait time for master (in seconds): ");
    scanf("%d", &masWait);

    pthread_create(&Master, NULL, master, (void *) 1);
    for(int i = 0; i < numSlaves; i++) {
        pthread_create(&Slaves[i], NULL, slave, (void *) i);
    }
    //Join the thread where it is left
    pthread_join(Master,NULL);
    for(int i = 0; i < numSlaves; i++) {
        pthread_join(Slaves[i],NULL);
    }
    
    exit(0);    //not reached
}