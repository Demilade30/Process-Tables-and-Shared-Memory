#include<stdio.h>
#include<stdlib.h>

char *program;

int main(int argc, char** argv){
	program = argv[0];
	printf("Hello World from %s\n",program);

	return EXIT_SUCCESS;
}
