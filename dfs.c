#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 4

int main(int argc, char *argv[]) {

	int size = 1;
    	int* arr = (int*)malloc(size * sizeof(int));

	int PN = atoi(argv[1]);
	int num;

	FILE *readFile;
	readFile = fopen("numbers.txt","r");
	if(!readFile){
		printf("Error: could not read file.\n");
		return 1;
	}
	int index = 0;
	while (fscanf(readFile, "%d", &num) != EOF) {
		if(index>=size){
			size++;
			arr = (int*)realloc(arr,size * sizeof(int));
		}
		arr[index] = num;
		index++;
        }
	int interval = index / PN;
	int start = 0;
	for(int i = 0;i<PN-1;i++){
    		pid_t res = fork();
    		if(res==0){ //child
    			printf("c read from %d to %d\n",start,start+interval);
    			exit(i);
    		}else{ //parent
    			int status;
    			waitpid(res,&status,0);
    			printf("p read from %d to %d\n",start+interval,start+interval+interval);
    		}
    		start += interval;
	}

	return 0;
}
