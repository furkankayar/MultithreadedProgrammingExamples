/*
  single_line_bridge.c

  There are fixed number of cars that are waiting to the pass other side of the bridge.
  Directions are randomly chosen.
  If any car is running on the bridge, cars that will go to opposite direction have to wait.
  But the cars that will go to same direction can enter the bridge while bridge capacity is not full.

  > gcc -pthread single_line_bridge.c -o single_line_bridge.out
  > ./single_line_bridge.out

  THIS SOLUTION CAN HAVE PROBLEMS!!!!

  Writed by: Furkan Kayar
  15.11.2019
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define CAR_NUMBER 20
#define BRIDGE_CAPACITY 2
#define UNDEFINED -1
#define WEST 0
#define EAST 1
#define TRUE 1
#define FALSE 0

sem_t bridge_lock;
sem_t mutex;
int car_on_bridge = 0;
int bridge_direction = UNDEFINED;

void* pass_bridge(void* id){

  int car_id = *((int*)id);
  int car_direction = rand() % 2;

  printf("CAR %d: Waiting to pass to %s.\n", car_id, car_direction == WEST ? "WEST" : "EAST");

  while(TRUE){

    sem_wait(&mutex);
    if(bridge_direction == UNDEFINED || car_on_bridge == 0)
      bridge_direction = car_direction;
    sem_post(&mutex);

    sem_wait(&bridge_lock);
    if(car_direction == bridge_direction){

      sem_wait(&mutex);
      car_on_bridge++;
      printf("CAR %d: Running on the bridge %s. Car on the bridge: %d\n", car_id, car_direction == WEST ? "WEST" : "EAST",car_on_bridge);
      sem_post(&mutex);
      sleep(1);
      sem_wait(&mutex);
      car_on_bridge--;
      printf("CAR %d: Reached to %s. Car on the bridge: %d\n", car_id, car_direction == WEST ? "WEST" : "EAST", car_on_bridge);
      if(car_on_bridge == 0)
        bridge_direction = UNDEFINED;
      sem_post(&mutex);
      sem_post(&bridge_lock);
      pthread_exit(NULL);
    }
    sem_post(&bridge_lock);
  }
}


int main(){

  int i;
  pthread_t thread_id[CAR_NUMBER];
  srand(NULL);

  sem_init(&bridge_lock, 0, BRIDGE_CAPACITY);
  sem_init(&mutex, 0, 1);

  for(i=0; i < CAR_NUMBER ; i++){

    int* direction = (int*)malloc(sizeof(int*));
    *direction = i + 1;
    pthread_create(&thread_id[i], NULL, pass_bridge, direction);

  }

  for(i=0 ; i < CAR_NUMBER ; i++){

    pthread_join(thread_id[i], NULL);
  }

}
