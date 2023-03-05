#ifndef CONSTANT_H
#define CONSTANT_H

#define MAX_SIMUL_PROCESSES 20

#define DEFAULT_SEC_INTERVAL 1
#define DEFAULT_NSEC_INTERVAL 1000000

const key_t keySHM = 10032023;

struct PCB{
	int occupied;
	pid_t pid;
	int startSeconds;
	int startNano;
};


#endif
