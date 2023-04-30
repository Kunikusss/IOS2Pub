#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>


sem_t *mutex;
sem_t *mutex2;
sem_t *lock;

sem_t *customerLine1;
sem_t *customerLine2;
sem_t *customerLine3;
int *LineNum;
int *closedFlag;

int *Line1Num;
int *Line2Num;
int *Line3Num;

//sem_t *mutex;
//sem_t *mutex;
//sem_t *mutex;


FILE *file;

void semaphore_init(){
   mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
   mutex2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
   lock = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
   customerLine1 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
   customerLine2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
   customerLine3 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
   sem_init(mutex, 1, 1);
   sem_init(mutex2, 1, 1);
   sem_init(lock, 1, 1);
   sem_init(customerLine1, 1, 0);
   sem_init(customerLine2, 1, 0);
   sem_init(customerLine3, 1, 0);
}

void cleanup_semaphores(){
   sem_destroy(mutex);
   sem_destroy(mutex2);
   sem_destroy(lock);
   sem_destroy(customerLine1);
   sem_destroy(customerLine2);
   sem_destroy(customerLine3);
   munmap(mutex, sizeof(sem_t));
   munmap(mutex2, sizeof(sem_t));
   munmap(lock, sizeof(sem_t));
   munmap(customerLine1, sizeof(sem_t));
   munmap(customerLine2, sizeof(sem_t));
   munmap(customerLine3, sizeof(sem_t));
}

void my_print(const char * format, ...){   
   sem_wait(mutex);
   va_list args;
   va_start (args, format);
   fprintf(file,"%d: ", *LineNum);
   (*LineNum)++;
   vfprintf(file,format,args);
   fflush(file);
   va_end(args);
   sem_post(mutex);
}


void processCustomer(int id){
   //sem_wait(mutex2);
   int random_num = (rand() % 3) + 1;
   my_print("Z %d: entering office for a service %d\n", id, random_num);

   if (random_num == 1)
   {
      sem_wait(mutex);
      (*Line1Num)++;
      sem_post(mutex);
      
      sem_wait(customerLine1);
      my_print("Z %d: called by office worker\n", id);
      //sem_post(mutex2);


   }
   else if (random_num == 2)
   {
      sem_wait(mutex);
      (*Line2Num)++;
      sem_post(mutex);

      sem_wait(customerLine2);
      my_print("Z %d: called by office worker\n", id);
     // sem_post(mutex2);
      

   }
   else if (random_num == 3)
   {
      sem_wait(mutex);
      (*Line3Num)++;
      sem_post(mutex);

      sem_wait(customerLine3);  
      my_print("Z %d: called by office worker\n", id);
     // sem_post(mutex2);

   }


   usleep((rand() % ((10)+1) + (0)) * 1000);
   return;

}

void processWorker(int id, long TU){

   while( !(*closedFlag) || !((*Line1Num ) <= 0 && (*Line2Num ) <= 0 && (*Line3Num ) <= 0)){
      int random_num = (rand() % 3) + 1;


      if((*Line1Num ) != 0 && random_num==1){
         

         sem_post(customerLine1);
         //sem_wait(mutex2);
         my_print("U %d: serving a service of type 1\n", id);
        
         
         usleep((rand() % 11) * 1000);

         my_print("U %d: service finished\n", id);

         sem_wait(mutex);
         (*Line1Num)--;
         sem_post(mutex);

      } else if((*Line2Num) != 0 && random_num==2){


         sem_post(customerLine2);
         //sem_wait(mutex2);
         my_print("U %d: serving a service of type 2\n", id);

         
         usleep((rand() % 11) * 1000);


         my_print("U %d: service finished\n", id);

         sem_wait(mutex);
         (*Line2Num)--;
         sem_post(mutex);

      } else if((*Line3Num) != 0 && random_num==3){
         
         
         sem_post(customerLine3);
         //sem_wait(mutex2);
         my_print("U %d: serving a service of type 3\n", id);

         usleep((rand() % 11) * 1000);

         my_print("U %d: service finished\n", id);

         sem_wait(mutex);
         (*Line3Num)--;
         sem_post(mutex);


      }else if((*Line1Num) == 0 && (*Line2Num) == 0 && (*Line3Num) == 0){
         my_print("U %d: taking break\n", id);

         usleep((rand() % ((TU)+1) + (0)) * 1000);

         my_print("U %d: break finished\n", id);
      } 

      
      if((*closedFlag) && ((*Line1Num ) == 0 && (*Line2Num ) == 0 && (*Line3Num ) == 0)){
         //sem_post(mutex);
         //sem_post(mutex2);
         //sem_post(customerLine1);
         //sem_post(customerLine2);
         //sem_post(customerLine3); 
         return;
      }
      

   }
   return;
}



int main(int argc, char *argv[]) {
   
   if (argc != 6){
      fprintf(stderr,"invalid amount of arguments\n");  
      exit (1);
      return 1;
   }





   if(!(strtol(argv[2], NULL, 10))){
      fprintf(stderr,"invalid argument\n");  
      exit (1);
      return 1;
    }     
 

   for (int argvIndex = 1; argvIndex < argc; argvIndex++)
   {
      char *endptr;
      strtol(argv[argvIndex], &endptr, 10);
      if (*endptr != '\0')
      {
         fprintf(stderr, "invalid argument: %s\n", argv[argvIndex]);
         exit(1);
      }
   }


   

   for (int argvIndex = 1; argvIndex < argc; argvIndex++)
   {
      if((strtol(argv[argvIndex], NULL, 10)) < 0){
         fprintf(stderr,"invalid argument (negative)\n");  
         exit (1);
         return 1;
      }     
   }

   
   long NZ = strtol(argv[1], NULL, 10);
   long NU = strtol(argv[2], NULL, 10);
   long TZ = strtol(argv[3], NULL, 10);
   long TU = strtol(argv[4], NULL, 10);
   long F  = strtol(argv[5], NULL, 10);

   if(TZ > 10000){
      fprintf(stderr,"TZ too high\n");  
      exit (1);
      return 1;
   }

   if(TU > 100){
      fprintf(stderr,"TU too high\n");  
      exit (1);
      return 1;
   }

   if(F > 10000){
      fprintf(stderr,"TU too high\n");  
      exit (1);
      return 1;
   }



   LineNum = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
   *LineNum = 1;
   closedFlag = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
   *closedFlag = 0;
   Line1Num = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
   *Line1Num = 0;
   Line2Num = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
   *Line2Num = 0;
   Line3Num = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
   *Line3Num = 0;
   int ForkNum ;

   
   


   file = fopen("proj2.out","w");
   if (file == NULL){
      fprintf(stderr, "File not recognized.");
      return 1;
   }


   semaphore_init();

   for (ForkNum=1; ForkNum <= NU; ForkNum++){
      pid_t id = fork();
      if (id == -1){
         sem_post(mutex);
         sem_post(mutex2);
         sem_post(customerLine1);
         sem_post(customerLine2);
         sem_post(customerLine3); 
      }
      
      if(id == 0){
         srand(time(NULL) + getpid());
         //(time(NULL) + getpid());
         //usleep((rand() % 11) * 1000);
         
         my_print("U %d: started\n", ForkNum);
         
         processWorker(ForkNum,TU);

        // if((*closedFlag) && ((*Line1Num ) == 0 && (*Line2Num ) == 0 && (*Line3Num ) == 0)) sem_post(mutex2);
            


         my_print("U %d: going home\n", ForkNum);
         exit(0);
      }
   }

   for (ForkNum=1; ForkNum <= NZ; ForkNum++){
      pid_t id = fork();
      if (id == -1){
         sem_post(mutex);
         sem_post(mutex2);
         sem_post(customerLine1);
         sem_post(customerLine2);
         sem_post(customerLine3); 
      }
      
      //printf();
      if(id == 0){
         //srand(time(NULL) + getpid());
         //usleep((rand() % 11) * 1000);
         srand(time(NULL) + getpid());
         my_print("Z %d: started\n", ForkNum);

         usleep((rand() % ((TZ)+1) + (0)) * 1000);

         if((*closedFlag)==0){ 
            processCustomer(ForkNum);
         } 

         my_print("Z %d: going home\n", ForkNum);
         exit(0);
         
      }
   }
   


    usleep((rand() % ((F/2)+1) + (F/2)) * 1000);
    *closedFlag = 1;

    my_print("closing\n");

    while (wait(NULL) > 0);

   cleanup_semaphores();
   munmap(LineNum, sizeof(int));
   munmap(closedFlag, sizeof(int));
   munmap(Line1Num, sizeof(int));
   munmap(Line2Num, sizeof(int));
   munmap(Line3Num, sizeof(int));

   
   fclose(file);
   return 0;

}
