#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <stdbool.h>
#include <signal.h>

/**
 * Take in two inputs H and PN,
 * compile : gcc -o DFS DFS.c
 * run: ./DFS 10 17
 * 
 * The above will compile our program and find 10 keys using 17 total process.
 * You can input any number up to 50 (can also go past 50 if u want) for PN;
*/
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
	writeFile = fopen("dfsOutput.txt","w");
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
	for(int i = 0;i<PN-1;i++){
    		pid_t res = fork();
    		if(res==0){ //child
    			int childSize = 1;
    			int* childArr = (int*)malloc(childSize * sizeof(int));
    			int childIndex = 0;
    			int childSum = 0;
    			int childMax = -1;
    			if(i==0){
    				fprintf(writeFile,"Hi I'm process %d with return arg %d and my parent is %d.\n",getpid(),i+1,getppid());
    				
    				for(int j = start;j<=(start+interval);j++){
    					if(arr[j]==-1){
    						if(childIndex>=childSize){
    							childSize++;
    							childArr = (int*)realloc(childArr,childSize * sizeof(int));
    						}
    						childArr[childIndex] = j;
    						childIndex++;
    					}else{
    						childSum += arr[j];
    						if(arr[j]>childMax){
    							childMax = arr[j];
    						}
    					}
    				}
    				int childAverage = childSum / ((interval+1) - childIndex);
    				
    				int childNumCount = ((interval+1) - childIndex);
    				
    				write(fd[1],&childAverage,sizeof(int));
    				write(fd[1],&childMax,sizeof(int));
    				write(fd[1],&childNumCount,sizeof(int));
    				
    				
    				pid_t this = getpid();
    				pid_t thisParent = i+1;
    				write(kp[1],&childIndex,sizeof(int));
    				write(kp[1],&this,sizeof(int));
    				write(kp[1],&thisParent,sizeof(int));
    				for(int j = 0;j<childIndex;j++){
    					int currKey = childArr[j];
    					write(kp[1],&currKey,sizeof(int));
    				}
    				
    				
    				exit(i+1);
    			}else{
    				fprintf(writeFile,"Hi I'm process %d with return arg %d and my parent is %d.\n",getpid(),i+1,getppid());
    				for(int j = start+1;j<=(start+interval);j++){
    					if(arr[j]==-1){
    						if(childIndex>=childSize){
    							childSize++;
    							childArr = (int*)realloc(childArr,childSize * sizeof(int));
    						}
    						childArr[childIndex] = j;
    						childIndex++;
    					}else{
    						childSum += arr[j];
    						if(arr[j]>childMax){
    							childMax = arr[j];
    						}
    					}
    				}
    				int childAverage = childSum / ((interval) - childIndex);
    				int childNumCount = ((interval) - childIndex);
    				
    				write(fd[1],&childAverage,sizeof(int));
    				write(fd[1],&childMax,sizeof(int));
    				write(fd[1],&childNumCount,sizeof(int));
    				pid_t this = getpid();
    				pid_t thisParent = i+1;
    				write(kp[1],&childIndex,sizeof(int));
    				write(kp[1],&this,sizeof(int));
    				write(kp[1],&thisParent,sizeof(int));
    				for(int j = 0;j<childIndex;j++){
    					int currKey = childArr[j];
    					write(kp[1],&currKey,sizeof(int));
    				}
    				exit(i+1);
    			
    			}
    		}else{ //parent
    			int parentSize = 1;
    			int* parentArr = (int*)malloc(parentSize * sizeof(int));
    			int parentIndex = 0;
    			int parentSum = 0;
    			int parentMax = -1;
    			int status;
    			//waitpid(res,&status,0);
    			if(i == PN -2){
    				
    				//close(fd[1]); //close writeEnd
    				read(fd[0],&subAvg,sizeof(int));
    				read(fd[0],&subMax,sizeof(int));
    				read(fd[0],&subCount,sizeof(int));
    				//close(fd[0]); //close readEnd
    				
    				
    				if(subMax>masterMax){
    					masterMax = subMax;
    				}
    				masterSum += (subAvg * subCount);
    				masterCount += subCount;
    				
    				for(int j = start+interval+1;j<=(index-1);j++){
    					if(arr[j]==-1){
    						if(parentIndex>=parentSize){
    							parentSize++;
    							parentArr = (int*)realloc(parentArr,parentSize * sizeof(int));
    						}
    						parentArr[parentIndex] = j;
    						parentIndex++;
    					}else{
    						parentSum += arr[j];
    						if(arr[j]>parentMax){
    							parentMax = arr[j];
    						}	
    					}
    				}
    				int parentAverage = parentSum / ((index - start - interval - 1 )-parentIndex);
    				
    				pid_t this = getpid();
    				pid_t thisParent = 0;
    				write(kp[1],&parentIndex,sizeof(int));
    				write(kp[1],&this,sizeof(int));
    				write(kp[1],&thisParent,sizeof(int));
    				
    				
    				for(int j = 0;j<parentIndex;j++){
    					int currKey = parentArr[j];
    					write(kp[1],&currKey,sizeof(int));
    				}
    				
    				if(parentMax>masterMax){
    					masterMax = parentMax;
    				}
    				masterSum += (parentAverage * ((index - start - interval - 1 )-parentIndex));
    				masterCount += ((index - start - interval - 1 )-parentIndex);
    				
    				fprintf(writeFile,"Max = %d, Avg = %d.\n",masterMax,masterSum/masterCount);
    				int loopNum;
    				int currPID;
    				int currPPID;
    				int keysWritten = 0;
    				
    				for(int k = 0;k<PN;k++){
    					read(kp[0],&loopNum,sizeof(int));
    					read(kp[0],&currPID,sizeof(int));
    					read(kp[0],&currPPID,sizeof(int));
    					int keyPosition;
    					for(int l = 0;l<loopNum;l++){
    						read(kp[0],&keyPosition,sizeof(int));
    						if(keysWritten<H){
    							fprintf(writeFile,"Hi i'm process %d with return arg %d. I found a hidden key in position A[%d].\n",currPID,currPPID,keyPosition);
    							keysWritten++;
    						}else{
    							k = PN;
    							l = loopNum;
    						}
    					}
    				}
    				clock_t end = clock();
    				double cpu_time_used = ((double) (end - start1)) / CLOCKS_PER_SEC;
    	printf("Elapsed time: %f seconds\n", cpu_time_used);
    			}else{
    				
    				//close(fd[1]); //close writeEnd
    				read(fd[0],&subAvg,sizeof(int));
    				read(fd[0],&subMax,sizeof(int));
    				read(fd[0],&subCount,sizeof(int));
    				//close(fd[0]); //close readEnd
    				
    				if(subMax>masterMax){
    					masterMax = subMax;
    				}
    				masterSum += (subAvg * subCount);
    				masterCount += subCount;
    				
    			}
    		}
    		start += interval;
	}
	fclose(writeFile);
	return 0;
}
