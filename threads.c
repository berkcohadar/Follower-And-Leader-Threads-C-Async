// Ubuntu 
// g++ 150160224.c -pthread

#include <pthread.h>
#include <semaphore.h> 
#include <unistd.h> 
#include <stdio.h>
#include <iostream>
#include <stdlib.h> 

sem_t followers, leaders, mutex; //followers, leaders, mutex semaphore
int followcount = 0;
int leadercount = 0;
bool lock = false;

void randomDelay() {
  srand(time(NULL));
  int x = rand() % (2000000 + 1);
  usleep(x);
}

void* followerThread(void *tmp) {
  randomDelay();
  sem_wait(&mutex);
  followcount++;
  sem_post(&followers);
  int val;
  sem_getvalue(&leaders,&val);
  if (val>0) sem_post(&leaders);
  else sem_post(&mutex);
  sem_wait(&followers);
  if (leadercount>0 && followcount>0) printf("Leader %d and Follower %d are dancing together now.\n",leadercount,followcount);
  lock = true;
  while(lock==true);
  if (leadercount>0 && followcount>0) {
    printf("Leader %d and Follower %d left the stage.\n",leadercount,followcount);
    leadercount--;
    followcount--;
  }
  pthread_exit(0);
}

void* leaderThread(void *tmp){
  randomDelay();
  sem_wait(&mutex);
  leadercount++;
  sem_post(&leaders);
  int val;
  sem_getvalue(&followers,&val);
  if (val>0) sem_wait(&followers);
  else sem_post(&mutex);
  sem_wait(&leaders);
  if (leadercount>0 && followcount>0) printf("Leader %d and Follower %d are dancing together now.\n",leadercount,followcount);
  while (lock == false);
  if (leadercount>0 && followcount>0) {
    printf("Leader %d and Follower %d left the stage.\n",leadercount,followcount);
    leadercount--;
    followcount--;
  }
  lock = false;
  sem_post(&mutex);
}
int main(int argc, char* argv[]) { 
  int leader_size = atoi(argv[1]);
  int follower_size = atoi(argv[2]);
  int i;
  pthread_t leader_thread[leader_size];
  pthread_t follower_thread[follower_size];

  sem_init(&mutex,0,1);
  sem_init(&followers,0,0);
  sem_init(&leaders,0,0);

  for(i = 0; i < leader_size; i++)  pthread_create(&leader_thread[i],NULL,leaderThread,(void*)&i);
  for(i = 0; i < follower_size; i++)  pthread_create(&follower_thread[i],NULL,followerThread,(void*)&i);
  for(i = 0; i < leader_size; i++) pthread_join(leader_thread[i], NULL);
  for(i = 0; i < follower_size; i++) pthread_join(follower_thread[i], NULL);
  printf("Done!\n");
  exit(true);
  } 