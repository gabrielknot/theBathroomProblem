#include <pthread.h> 
#include <stdlib.h>
#include <stdio.h>
#include<semaphore.h>

#define QTD_OF_PEOPLE 10
#define QTD_OF_BOXES 5

int sharedData = 0; 
typedef struct{
	int id;
  unsigned short int gender;
}people;

sem_t total_boxes;

sem_t gender_mutex;

pthread_mutex_t mutex;
unsigned short int current_gender;

void delay(int secs) { //utility function
  time_t beg = time(NULL), end = beg + secs;
  do ; while (time(NULL) < end);
}
char *getGender(int i){
  if(i)
    return "woman";
  else
  {
    return "men";
  }
  
}
void poopiing(people*philo){
  int box;

  sem_getvalue(&total_boxes,&box);

  printf("the %s #%d, pooping at box #%d\n",getGender(philo->gender),philo->id,box);
  delay(rand() % 5);
  printf("the %s #%d, done pooping at box #%d\n",getGender(philo->gender),philo->id,box);
  sem_post(&total_boxes);
}

//Try commenting out the calls to the lock and unlock functions.
void *bathroom(void *philo) {
 //Obtain exclusive mutex lock
  people *peop_cast = (people*)philo;
  int free_boxes;
  
  if(current_gender!=peop_cast->gender){
    delay(rand() % 5);
    printf("Im the %s #%d, the bathroom is %s's avaliable for while...\n",getGender(peop_cast->gender),peop_cast->id,getGender(!peop_cast->gender));
    sem_wait(&gender_mutex);
    pthread_mutex_lock(&mutex);
    printf("\nThe %s #%d - Boxes are free for my gender. Finaly i can change get in\n",getGender(peop_cast->gender),peop_cast->id);
    current_gender=!current_gender;
    pthread_mutex_unlock(&mutex);
  }
  
  sem_wait(&total_boxes);
  poopiing(peop_cast);

  pthread_mutex_lock(&mutex);
  sem_getvalue(&total_boxes,&free_boxes);
  if(free_boxes==QTD_OF_BOXES){
    sem_post(&gender_mutex);
    }
  pthread_mutex_unlock(&mutex);
  
  return philo;
}

int main() {
people* peoples = (people *) malloc(sizeof(people)*QTD_OF_PEOPLE);
  pthread_attr_t attr;
  pthread_attr_init(&attr); //initialise thread attribute data ...
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

  sem_init(&total_boxes,0,QTD_OF_BOXES);
  sem_init(&gender_mutex,0,0);
  pthread_mutex_init(&mutex,NULL);

  pthread_t people_thrds[QTD_OF_PEOPLE]; //Data strs form managing several threads

  int t;

  srand(time(NULL)); // initialise random num generator

  
  for (t=0; t<QTD_OF_PEOPLE; t++) {//create & initialise several thread instances
    peoples[t].id = t;
    peoples[t].gender = rand() % 2;
    if(t==0)
      current_gender=peoples[t].gender;
    printf("\nThe %s #%d is in the line\n",getGender(peoples[t].gender), t);
    pthread_create(&people_thrds[t], &attr, bathroom, &peoples[t]);
  }

  for (t=0; t<QTD_OF_PEOPLE; t++) //initialise the semaphores
    pthread_join(people_thrds[t], NULL);
  
  free(peoples);
  sem_destroy(&total_boxes);
  sem_destroy(&gender_mutex);
  pthread_mutex_destroy(&mutex);
  pthread_attr_destroy(&attr);
}