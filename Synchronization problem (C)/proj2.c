/*
 * File: proj2.c
 * Date: 04.04.2016
 * Author: Ján Gula,xgulaj00
 * Project: IOS-DU2
 * Compiler: GNU GCC
 * OS: Windows 10
 * Description: Modified synchronization problem "The Roller Coaster Problem".
*/

#include "proj2.h"

sem_t	*boarding,
	*boarded,
	*landing, 
	*landed,
	*waiting,
	*pcounter,
	*writing;

int	shm_pid = 0,
	shm_pass_id=0,
	shm_d = 0,
	shm_b = 0;

int	*process_id = NULL, 
	*passenger_id = NULL,
	*done = NULL,
	*boarded_id = NULL;

pid_t pid_car, pid_passenger;

FILE *file = NULL;

int main(int argc, char *argv[]) {
	
	// Testing if the arguments are in the correct format.
	if (invalid_input(argc,argv) == 1) {
		return 1;
	}
	int P, C, PT,RT;
	
	P = strtol(argv[1],NULL,0);
	C = strtol(argv[2],NULL,0);
	PT = strtoul(argv[3],NULL,0);
	RT = strtoul(argv[4],NULL,0);

	// Testing if parameters are in the correct range.
	if (invalid_arguments(P,C,PT,RT) == 1) {
		return 1;
	}

	// Signal handlers
	signal(SIGTERM, terminate_program);
	signal(SIGINT, terminate_program);
	
	// To generate random numbers
	srand(time(0));

	if((file = fopen("proj2.out", "w")) == NULL) {
		fprintf(stderr, "File opening failed.\n");
		return 1;
	} 

	setbuf(file, NULL);

	int children[P];

	int resources = set_resources();
	
	// Testing if set_resources worked properly
	if (resources != NO_ERROR) {
		if (resources == ERROR_SEM) {
			fprintf(stderr, "Creating a semaphore failed.\n");
			return ERROR_SEM;
		} else {
			fprintf(stderr, "Allocating memory failed.\n");
			return ERROR_SHM;
		}
		free_resources();
	}

	int process_time = 0,
		i = 0;

	// Initializing counters
	*passenger_id = 1;
	*process_id = 1;
	*done = 1;
	*boarded_id = 1;

	pid_car = fork();
	
	// Child ( Passenger generating process )
	if (pid_car == 0) {
	  for(i = 0; i < P; i++) {
	  	process_time = (random() % (PT + 1));
	  	usleep(process_time * 1000);
	  	pid_passenger = fork();
	  	if(pid_passenger == 0) {
	  		passenger(C);
	  		break;
	  	} else if (pid_passenger > 0) {
	  		children[i] = pid_passenger;
	  			} else {
	  				fprintf(stderr, "Error creating a passenger.\n");
	  				terminate_program();
	  				}
	  }
	// Parent ( Car )
	} else if (pid_car > 0) {
	  car(P,C,RT);
	  children[i] = pid_car;
	  } else {
	  	fprintf(stderr, "Error creating a car.\n");
	  	terminate_program();
	}

	// Waiting for passengers
	for(i = 0; i < P; i++) 
		waitpid(children[i], NULL, 0);

	free_resources();
	return 0;
	}


void car(int P, int C, int RT) {
	
	sem_wait(writing);		
	fprintf(file, "%d	: C 1	: started\n",inc_processid(process_id));
	sem_post(writing);

	fflush(file);
		// Riding cycle ( P/C = Number of rides )
		for(int j = 0; j < P/C; j++) {

			sem_wait(writing);		
			fprintf(file, "%d	: C 1	: load\n",inc_processid(process_id));
			sem_post(writing);
			
			fflush(file);

			// Let "C" processes through the boarding semaphore
			for(int i = 0; i < C; i++)
				sem_post(boarding);

			sem_wait(boarded); // Wait for everyone to board
			
			sem_wait(writing);
			fprintf(file, "%d 	: C 1	: run\n",inc_processid(process_id));
			sem_post(writing);
			
			fflush(file);

			int process_time = 0;

			if(RT != 0) {
				process_time = (random() % (RT + 1));
		  	} else {
		  		process_time = 0;
		  	}
		  	usleep(process_time * 1000); // HOORAY FOR A RIDE ( process sleeps )
			
	  		sem_wait(writing);
			fprintf(file, "%d 	: C 1	: unload\n",inc_processid(process_id));
			sem_post(writing);
		
			fflush(file);
			
			// Let "C" processes through the landing semaphore
	  		for(int i = 0; i < C; i++)
				sem_post(landing);
	  	
			sem_wait(landed);
		}
	sem_post(waiting); 	// Set the finishing semaphore to 1 so that everyone could start finishing
	
	sem_wait(writing);
	fprintf(file, "%d 	: C 1	: finished\n",inc_processid(process_id));
  	sem_post(writing);

  	fflush(file);
  	exit(0);
}	

	void passenger(int C) {
		int collision_proof = inc_processid(passenger_id); // Each passenger obtains his own collision_proof id 
		
		sem_wait(writing);
		fprintf(file, "%d 	: P %d 	: started\n",inc_processid(process_id),collision_proof);
		sem_post(writing);

		fflush(file);

		sem_wait(boarding); // Wait for the car to start boarding
		
		sem_wait(writing);		
		fprintf(file, "%d 	: P %d 	: board\n",inc_processid(process_id),collision_proof);
		int boardc = inc_processid(boarded_id);
		sem_post(writing);

		fflush(file);

		// If this is the last passenger before the car is full
		if(boardc == C) {
			*boarded_id = 1;

			sem_wait(writing);		
			fprintf(file, "%d 	: P %d 	: board last\n",inc_processid(process_id),collision_proof);
			sem_post(writing);

			fflush(file);

			sem_post(boarded); // Let the car know that everyone's boarded
		} else {
			sem_wait(writing);		
			fprintf(file, "%d 	: P %d 	: board order %d \n",inc_processid(process_id),collision_proof,boardc);
			sem_post(writing);	

			fflush(file);	
		}
		sem_wait(landing); // Wait for the car to finish the ride and start unloading

		sem_wait(writing);		
		fprintf(file, "%d 	: P %d 	: unboard\n",inc_processid(process_id),collision_proof);
		int unboardc = inc_processid(done);
		sem_post(writing);

		fflush(file);	
		// If this is the last passanger that exits the car
		if(unboardc == C) {
			*done = 1;
			
			sem_wait(writing);		
			fprintf(file, "%d 	: P %d 	: unboard last\n",inc_processid(process_id),collision_proof);
			sem_post(writing);
			
			fflush(file);
			
			sem_post(landed); // Let the car know that everyone landed
		} else {
			sem_wait(writing);		
			fprintf(file, "%d 	: P %d 	: unboard order %d \n",inc_processid(process_id),collision_proof,unboardc);
			sem_post(writing);	
			
			fflush(file);	
		}	
		sem_wait(waiting); // Wait for the car to finish the ride 
		
		sem_wait(writing);
		fprintf(file, "%d 	: P %d 	: finished\n",inc_processid(process_id),collision_proof);
		sem_post(writing);
		
		sem_post(waiting); // Let the next process finish
		
		fflush(file);
		exit(0);
		}

	int set_resources(void) {
		// Initializes memory map for all semaphores 
		if((boarding = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { return ERROR_SHM; }
		if((boarded = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { return ERROR_SHM; }
		if((landing = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { return ERROR_SHM; }
		if((landed = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { return ERROR_SHM; }
		if((waiting = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { return ERROR_SHM; }
		if((pcounter = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { return ERROR_SHM; }		
		if((writing = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) { return ERROR_SHM; }

		// Initializes semaphores
		if(sem_init(boarding, 1, 0) == -1) { return ERROR_SEM;}	
		if(sem_init(boarded, 1, 0) == -1) { return ERROR_SEM;}	
		if(sem_init(landing, 1, 0) == -1) { return ERROR_SEM;}	
		if(sem_init(landed, 1, 0) == -1) { return ERROR_SEM;}	
		if(sem_init(waiting, 1, 0) == -1) { return ERROR_SEM;}	
		if(sem_init(pcounter, 1, 1) == -1) { return ERROR_SEM;}	
		if(sem_init(writing, 1, 1) == -1) { return ERROR_SEM;}
		
		if((shm_pid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1) { return ERROR_SHM; }
		if((process_id = (int *) shmat(shm_pid, NULL, 0)) == NULL) { return ERROR_SHM; }

		if((shm_pass_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1) { return ERROR_SHM; }
		if((passenger_id = (int *) shmat(shm_pass_id, NULL, 0)) == NULL) { return ERROR_SHM; }

		if((shm_d = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1) { return ERROR_SHM; }
		if((done = (int *) shmat(shm_d, NULL, 0)) == NULL) { return ERROR_SHM; }

		if((shm_b = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1) { return ERROR_SHM; }
		if((boarded_id = (int *) shmat(shm_b, NULL, 0)) == NULL) { return ERROR_SHM; }

		return NO_ERROR;
	}

// Cleaning up
void free_resources() {
	int error;
	error = NO_ERROR;
	fclose(file);
	// Destroying semaphores
	if(sem_destroy(boarding) == -1) { error = ERROR_SEM; }
	if(sem_destroy(boarded) == -1) { error = ERROR_SEM; }
	if(sem_destroy(landing) == -1) { error = ERROR_SEM; }
	if(sem_destroy(landed) == -1) { error = ERROR_SEM; }
	if(sem_destroy(waiting) == -1) { error = ERROR_SEM; }
	if(sem_destroy(pcounter) == -1) { error = ERROR_SEM; }
	if(sem_destroy(writing) == -1) { error = ERROR_SEM; }
	
	if(error == ERROR_SEM) { fprintf(stderr, "Deleting semaphores failed.\n"); }
	
	// Releasing shared memory 
	if(shmctl(shm_pid, IPC_RMID, NULL) == -1) { error = ERROR_SHM; }
	if(shmctl(shm_d, IPC_RMID, NULL) == -1) { error = ERROR_SHM; }
	if(shmctl(shm_b, IPC_RMID, NULL) == -1) { error = ERROR_SHM; }
	if(shmctl(shm_pass_id, IPC_RMID, NULL) == -1) { error = ERROR_SHM; }

	if(error == ERROR_SHM) { fprintf(stderr, "Memory deallocation failed.\n"); }
}

// Action counter
int inc_processid(int *procid) { 
	int original = 0;
	sem_wait(pcounter); 
	original = *procid;   
	(*procid)++; 
    sem_post(pcounter);   
    return original;
}

// If anything went wrong free everything and kill zombie processes
void terminate_program() {
		free_resources();
		kill(pid_car, SIGTERM);
		kill(getpid(), SIGTERM);
		exit(1);
	}	
