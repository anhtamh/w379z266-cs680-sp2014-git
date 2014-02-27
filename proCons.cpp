/************************************************
** CS540 - Producer & Consumer program         **
** Name: Huynh, Tam                            **
** Lab id: txhuynh                             **
** WSUID: W379Z266                             **
************************************************/

#include <semaphore.h>
#include <pthread.h>
#include <iostream>
#include <cstdlib>

#define FACTOR 0.000000001 // one-billionth
#define SIZE 5

using namespace std;

int buffer[2][SIZE];// buffer[0] and buffer[1] are two INDEPENDENT buffers
void insertItem(int buffNum, int item);// inserts item into buffer[buffNum]
void removeItem(int buffNum, int *item);// removes an item from buffer[buffNum]

void* producer(void *prod);// 1 <= prodId <= numProducers
void* consumer(void *cons);// 1 <= consId <= numConsumers

pthread_t tid;
pthread_attr_t attr;

sem_t empty[2], full[2];
pthread_mutex_t mutex[2];

int in[2] = {0}, out[2] = {0};
int counter = 0; //number of unconsumed item

int main(int argc, char *argv[]) {
    //1. Get command line arguments argc, argv[1],argv[2],argv[3]

    //2. Check for errors in the arguments; if any errors, output appropriate error message and exit.
    if (argc != 4) {
        cout << "Invalid arguments.\nRequires: <int>_<int>_<int>\n";
        exit(1);
    }

    //3. Assign argv[1], argv[2] and argv[3] to sleepTime, numProducers, and numConsumers, respectively.
    int sleepTime = atoi(argv[1]);
    int numProducers = atoi(argv[2]);
    int numConsumers = atoi(argv[3]);

    //4. Initialize 2 buffers with -1
    for (int buff = 0; buff < 2; buff++)
        for (int i = 0; i < SIZE; i++)
            buffer[buff][i] = -1;

    //5. Create and initialize full, empty and mutex
    sem_init(&empty[0], 0, SIZE); // create the semaphore empty and initialize it to SIZE
    sem_init(&full[0], 0, 0); // create the semaphore full and initialize it to 0
    pthread_mutex_init(&mutex[0],NULL); // create and initialize the mutex lock

    sem_init(&empty[1], 0, SIZE); // create the semaphore empty and initialize it to SIZE
    sem_init(&full[1], 0, 0); // create the semaphore full and initialize it to 0
    pthread_mutex_init(&mutex[1],NULL); // create and initialize the mutex lock

    pthread_attr_init(&attr);

    //srand(time(NULL));//random seed with current time
    cout << endl;
    //6. Create numProducers producer thread(s)
    for (int i = 1; i <= numProducers; i++) {
        pthread_create(&tid, &attr, producer, (void *)i);
    }

    //7. Create numConsumers consumer thread(s)
    for (int i = 1; i <= numConsumers; i++) {
        pthread_create(&tid, &attr, consumer, (void *)i);
    }

    //8. Sleep for sleepTime
    sleep(sleepTime);
    cout << endl;
    //9. Exit
    return 0;
}

// The producer and consumer threads use full and empty semaphores.
// They do NOT deal with mutex
// mutex is only used in insertItem() and removeItem()

void* producer(void *prod) {// 1 <= prodId <= numProducers
    int prodId = (int)prod;
    int buffNum, item;
    int x; // how long the producer sleeps
    while (1) {
        x = rand()*FACTOR;
        sleep(x); // sleep for x units of time
        item = rand(); // generate a random number as item
        buffNum = rand()%2; //insert into buffNum 0 or 1

        sem_wait(&empty[buffNum]); //get empty lock
        insertItem(buffNum, item);
        cout << "  Producer " << prodId << " inserted " << item << " into buffer " << buffNum << endl;
        sem_post(&full[buffNum]);//signal full lock
    }
}

void* consumer(void *cons) {// 1 <= consId <= numConsumers
    int consId = (int)cons;
    int buffNum, item;
    int x; // how long the consumer sleeps
    while (1) {
        x = rand()*FACTOR;
        sleep(x); // sleep for x units of time
        buffNum = rand()%2; //remove from buffNum 0 or 1

        sem_wait(&full[buffNum]);//get full lock
        removeItem(buffNum, &item);
        cout << "  Consumer " << consId << " consumed " << item << " from buffer " << buffNum << endl;
        sem_post(&empty[buffNum]);//signal empty lock
    }
}

void insertItem(int buffNum, int item) {
    pthread_mutex_lock(&mutex[buffNum]);//get mutex lock
    buffer[buffNum][in[buffNum]] = item;
    in[buffNum] = (in[buffNum]+1) % SIZE;
    counter++;
    pthread_mutex_unlock(&mutex[buffNum]);//release mutex lock
}

void removeItem(int buffNum, int *item) {
    pthread_mutex_lock(&mutex[buffNum]);//get mutex lock
    *item = buffer[buffNum][out[buffNum]];
    out[buffNum] = (out[buffNum]+1) % SIZE;
    counter--;
    pthread_mutex_unlock(&mutex[buffNum]);//release mutex lock
}
