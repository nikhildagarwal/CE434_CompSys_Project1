#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {

	int fd[2];
	int subAvg;
	int subMax;
	int subCount;
	int masterSum = 0;
	int masterCount = 0;
	int masterMax = -1;
	if (pipe(fd) == -1) {
        printf("Failed to create pipe\n");
        return 1;
    }
    
	int size = 1;
    int* arr = (int*)malloc(size * sizeof(int));

	int PN = atoi(argv[1]);
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
    				//close(fd[0]); //close readEnd
    				printf("iteration i = %d, c read from %d to %d\n",i,start+1,start+interval+1);
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
    				printf("%d\n",childAverage);
    				int childNumCount = ((interval+1) - childIndex);
    				
    				write(fd[1],&childAverage,sizeof(int));
    				write(fd[1],&childMax,sizeof(int));
    				write(fd[1],&childNumCount,sizeof(int));
    				
    				printf("Average = %d, Max = %d, numCount = %d\n",childAverage,childMax,childNumCount);
    				for(int j = 0;j<childIndex;j++){
    					printf("Key @ index %d\n",childArr[j]);
    				}
    				printf("\n");
    				
    				exit(i+1);
    			}else{
    				fprintf(writeFile,"Hi I'm process %d with return arg %d and my parent is %d.\n",getpid(),i+1,getppid());
    				printf("iteration i = %d, c read from %d to %d\n",i,start+1+1,start+interval+1);
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
    				
    				printf("Average = %d, Max = %d, numCount = %d\n",childAverage,childMax,childNumCount);
    				for(int j = 0;j<childIndex;j++){
    					printf("Key @ index %d\n",childArr[j]);
    				}
    				printf("\n");
    				exit(i+1);
    			
    			}
    		}else{ //parent
    			int parentSize = 1;
    			int* parentArr = (int*)malloc(parentSize * sizeof(int));
    			int parentIndex = 0;
    			int parentSum = 0;
    			int parentMax = -1;
    			int status;
    			waitpid(res,&status,0);
    			if(i == PN -2){
    			
    				printf("\nRead from pipe:\n");
    				//close(fd[1]); //close writeEnd
    				read(fd[0],&subAvg,sizeof(int));
    				read(fd[0],&subMax,sizeof(int));
    				read(fd[0],&subCount,sizeof(int));
    				//close(fd[0]); //close readEnd
    				
    				printf("SubAvg: %d\n",subAvg);
    				printf("SubMax: %d\n",subMax);
    				printf("SubNumbersCount: %d\n\n\n",subCount);
    				
    				if(subMax>masterMax){
    					masterMax = subMax;
    				}
    				masterSum += (subAvg * subCount);
    				masterCount += subCount;
    				
    				printf("iteration i = %d, p read from %d to %d\n",i,start+interval+1+1,index - 1+1);
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
    				printf("Sum %d\n",parentSum);
    				printf("Average = %d, Max = %d, numCount = %d\n",parentAverage,parentMax,((index - start - interval - 1 )-parentIndex));
    				for(int j = 0;j<parentIndex;j++){
    					printf("Key @ index %d\n",parentArr[j]);
    				}
    				printf("\n");
    				
    				if(parentMax>masterMax){
    					masterMax = parentMax;
    				}
    				masterSum += (parentAverage * ((index - start - interval - 1 )-parentIndex));
    				masterCount += ((index - start - interval - 1 )-parentIndex);
    				
    				printf("%d\n",masterSum/masterCount);
    				printf("%d\n",masterMax);
    				fprintf(writeFile,"Max = %d, Avg = %d.\n",masterMax,masterSum/masterCount);
    			}else{
    				
    				printf("\nRead from pipe:\n");
    				//close(fd[1]); //close writeEnd
    				read(fd[0],&subAvg,sizeof(int));
    				read(fd[0],&subMax,sizeof(int));
    				read(fd[0],&subCount,sizeof(int));
    				//close(fd[0]); //close readEnd
    				
    				printf("SubAvg: %d\n",subAvg);
    				printf("SubMax: %d\n",subMax);
    				printf("SubNumbersCount: %d\n\n\n",subCount);
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
