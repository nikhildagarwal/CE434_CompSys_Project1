#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <stdbool.h>

#define QUEUE_SIZE 100

int main(int argc, char *argv[]) {
	clock_t start1 = clock();
	int fd[2];
	int kp[2];
	int subAvg;
	int subMax;
	int subCount;
	int keysFound = 0;
	int masterSum = 0;
	int masterCount = 0;
	int masterMax = -1;
	if (pipe(fd) == -1) {
        printf("Failed to create pipe\n");
        return 1;
    } 
    if (pipe(kp) == -1){
    	printf("Failed to created pipe\n");
    	return 1;
    }
    
	int size = 1;
    int* arr = (int*)malloc(size * sizeof(int));

	int H = atoi(argv[1]);
	int PN = atoi(argv[2]);
	int num;
	
	FILE *writeFile;
	writeFile = fopen("bfsOutput.txt","w");
	fprintf(writeFile,"Hi I'm process %d with return arg %d and my parent is %d.\n",getpid(),0,getppid());
	
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

	// start of BFS implementation
    int queue[QUEUE_SIZE];
    int front = -10, rear = 10;
    int processCount = 0;

    // add root process to queue
    queue[++rear] = 0;

    while (front <= rear) {
        int currProcess = queue[front++]; // remove the first process in queue
        pid_t res = fork();
        if (res == 0) { //child
            int childSize = 1;
            int* childArr = (int*)malloc(childSize * sizeof(int));
            int childIndex = 0;
            int childSum = 0;
            int childMax = -1;

            if (currProcess == 0) {
                fprintf(writeFile,"Hi I'm process %d with return arg %d and my parent is %d.\n",getpid(),currProcess+1,getppid());
                for (int j = start; j <= (start+interval); j++) {
                    if (arr[j] == -1) {
                        if (childIndex >= childSize) {
                            childSize++;
                            childArr = (int*)realloc(childArr,childSize * sizeof(int));
                        }
                        childArr[childIndex] = j;
                        childIndex++;
                    } else {
                        childSum += arr[j];
                        if (arr[j] > childMax) {
                            childMax = arr[j];
                        }
                    }
                }
                int childAverage = childSum / ((interval+1) - childIndex);
                int childNumCount = ((interval+1) - childIndex);

                write(fd[1],&childAverage,sizeof(int));
                write(fd[1],&childMax,sizeof(int));
				write(fd[1],&childNumCount,sizeof(int));
				write(fd[1],&childIndex,sizeof(int));

				for (int k = 0; k < childIndex; k++) {
					write(fd[1],&childArr[k],sizeof(int));
				}

				fclose(writeFile);
				fclose(readFile);
				close(fd[0]);
				close(fd[1]);
				close(kp[0]);
				close(kp[1]);
				exit(0);
			}

			else { //parent process
				int parentAvg;
				int parentMax;
				int parentCount;
				int parentIndex;
				int parentKeys[QUEUE_SIZE];
				read(fd[0],&parentAvg,sizeof(int));
				read(fd[0],&parentMax,sizeof(int));
				read(fd[0],&parentCount,sizeof(int));
				read(fd[0],&parentIndex,sizeof(int));
				for (int k = 0; k < parentIndex; k++) {
					read(fd[0],&parentKeys[k],sizeof(int));
				}
				fclose(writeFile);
				fclose(readFile);
				close(fd[0]);
				close(fd[1]);
				close(kp[0]);
				close(kp[1]);
				exit(0);
			}
		}
		else if (res > 0) { //parent
			processCount++;
			if (processCount == PN) {
				for (int i = 0; i < processCount; i++) {
					int status;
					wait(&status);
				}
				FILE *readFile2;
				readFile2 = fopen("bfsOutput.txt","r");
				if(!readFile2){
					printf("Error: could not read file.\n");
					return 1;
				}
				char output[1000];
				while (fgets(output, 1000, readFile2) != NULL) {
					printf("%s", output);
				}
				fclose(readFile2);
				return 0;
			}
			continue;
			}
			else { //failed to fork
				printf("Failed to fork\n");
				return 1;
			}
    }
clock_t end = clock();
printf("Elapsed time: %f seconds\n", start1);
double cpu_time_used = ((double) (end - start1)) / CLOCKS_PER_SEC;
printf("Elapsed time: %f seconds\n", cpu_time_used);
fclose(writeFile);
fclose(readFile);
close(fd[0]);
close(fd[1]);
close(kp[0]);
close(kp[1]);
return 0;
}