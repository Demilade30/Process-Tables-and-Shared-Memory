#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include "constant.h"
#include <signal.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/shm.h>

char* program;

//user parameter
static int proc = 1;
static int simul = 1;
static int timeLimit = DEFAULT_SEC_INTERVAL;

//shareMemory
static struct timespec * shareClock = NULL;
int shmid = -1;

static struct PCB * pcb = NULL;

static void helpMenu(){
	printf("Usage: ./oss [-h] [-n proc] [-s simul] [-t iter]\n");
        printf("\t\t-h describes how the project should run and then, terminates.\n");
        printf("\t\t-n proc: the number of total children worker processes to launch.\n");
        printf("\t\t-s simul: the number of children processes that can be running simultaneously and.\n");
	printf("\t\t-t timeLimit: ceilling limit of the time interval that the oss will fork worker process\n");
	printf("\tIf any of the parameter above are not defined by user, the default value for them is 1\n");
}

static int createSHM(){
	shmid = shmget(keySHM, sizeof(struct timespec), IPC_CREAT | IPC_EXCL | S_IRWXU);
	if(shmid < 0){
		fprintf(stderr,"%s: failed to get id for shared memory. ",program);
                perror("Error");
                return -1;
	}

	shareClock = (struct timespec *)shmat(shmid,NULL,0);
	if(shareClock == NULL){
		fprintf(stderr,"%s: failed to get pointer for shared memory. ",program);
                perror("Error");
                return -1;
	}
	shareClock->tv_sec = 0;
	shareClock->tv_nsec = 0;

	pcb = (struct PCB *)malloc(sizeof(struct PCB) * simul);
	if(pcb == NULL){
		fprintf(stderr,"%s: failed to create process control block. ",program);
		perror("Error");
		return -1;
	}
	return 0;
}
static void deallocateSHM(){
	if(shareClock != NULL){
		if(shmdt(shareClock) == -1){
			fprintf(stderr,"%s: failed to detach shared memory. ",program);
                	perror("Error");
		}
	}

	if(shmid != -1){
		if(shmctl(shmid, IPC_RMID, NULL) == -1){
			fprintf(stderr,"%s: failed to delete shared memory. ",program);
                        perror("Error");
		}		
	}

	if(pcb != NULL)
		free(pcb);
}
int main(int argc, char** argv){
	program = argv[0];

	int opt = 0;
	//int userForked = 0;
	
	//Register exit function to deallocate memory
	atexit(deallocateSHM);

	while((opt = getopt(argc, argv, "hn:s:t:")) > 0){
		switch (opt){
    			case 'h':
      				helpMenu();
     		 		return -1;
    			case 'n':
				proc = atoi(optarg);
				break;
			case 's':
				simul = atoi(optarg);
				break;
			case 't':
				timeLimit = atoi(optarg);
                                break;
			case '?':
                                fprintf(stderr, "%s: ERROR: Unrecognized option: -%c\n",program,optopt);
                                return -1;
			default:
      				helpMenu();
      				return -1;
		}


	}

	if(simul > MAX_SIMUL_PROCESSES){
		printf("The number of simultaneous processes cannot be more than %d!\n",MAX_SIMUL_PROCESSES);
		return EXIT_FAILURE;
	}

	if(createSHM() == -1)
		return EXIT_FAILURE;

			
	

	return EXIT_SUCCESS;

}
