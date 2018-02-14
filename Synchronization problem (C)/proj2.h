/*
 * File: proj2.h
 * Date: 04.04.2016
 * Author: Ján Gula,xgulaj00
 * Project: IOS-DU2
 * Compiler: GNU GCC
 * OS: Windows 10
 * Description: Handling errors and declaring functions.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <signal.h>

#define NO_ERROR 0
#define ERROR_SEM 1
#define ERROR_SHM 2


// Functions declarations
void terminate_program();
int set_resources(void); // Allocates memory and initializes semaphores
void free_resources(void);
void car(int P,int C, int RT);
void passenger(int C);
int inc_processid(int *procid);

int invalid_input(int argc, char *argv[]) {
  if(argc != 5) {                                                    
      fprintf(stderr, "Invalid number of arguments.\n");                
      return 1;                                                      
  }                                                                 
  static int ac;                                                   
  static char *next;                                                
  next = NULL;                                                    
  for(ac=1; ac<argc; ac++){                                        
    strtol(argv[ac], &next, 10);                                  
    if((next == argv[ac]) || (*next != '\0')) {                   
      fprintf(stderr, "Argument %d is invalid.\n",ac);            
      return 1;                                                  
    }                                                             
    static long int arg_value;                                     
    arg_value = strtol(argv[ac],NULL,0);                            
    if(arg_value < 0) {                                            
      fprintf(stderr, "Argument %d is less than zero.\n",ac);        
      return 1;                                                    
    }                                    
  }
return 0;
}

int invalid_arguments(int P, int C, int PT, int RT) { 
  if (P == 0) {                                                      
     fprintf(stderr, "Argument P is equal to zero.\n");                
     return 1;                                                       
  }                                                                  
   if (C == 0) {                                                      
      fprintf(stderr, "Argument P is equal to zero.\n");                
      return 1;                                                       
  }                                                                  
  if (P <= C) {                                                      
    fprintf(stderr, "Argument P has to have a bigger value than argument C.\n");
    return 1;                                                       
      }                                                            
  if ((P % C) != 0) {                                                
   fprintf(stderr, "Argument P is not a multiple of argument C.\n"); 
   return 1;                                                       
   }                                                               
  if (PT >= 5001) {                                                  
    fprintf(stderr, "Argument PT is not in range of <0 - 5001).\n");  
    return 1;                                                       
    }                                                              
  if (RT >= 5001) {                                                   
    fprintf(stderr, "Argument RT is not in range of <0 - 5001).\n");  
    return 1;                                                       
   }
   return 0;
} 
