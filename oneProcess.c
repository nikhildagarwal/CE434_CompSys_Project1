#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

/*
*  Takes in one command line input --> H.
*  H = the number of keys we want to find.
*  Since this program is by default only running one process,
   we do not need to add a processNumber command line input.
*/
int main(int argc, char* argv[]){
	
	FILE *writeFile;
	writeFile = fopen("oneProcessOutput.txt","w");
	pid_t pid = getpid();
	pid_t ppid = getppid();
	fprintf(writeFile,"Hi I'm process %d with return arg 0 and my parent is %d.\n",pid,ppid);
	
	//H read from the command line
	int H = atoi(argv[1]);
	
	int num;
	
	FILE *readFile;
	readFile = fopen("numbers.txt","r");
	
	if(!readFile){
		printf("Error: could not read file.\n");
		return 1;
	}
	
	int index = 0;
	int sum = 0;
	int max = -1;
	int posIndex = 0;
	int keyPos[50];
	while (fscanf(readFile, "%d", &num) != EOF) {
		if(num == -1){
			keyPos[posIndex] = index;
			posIndex++;
		}
        	index++;
        	if(num>max){
        		max = num;
        	}
        	sum += num;
        }
        int average = sum / (index-50);
        
        fprintf(writeFile,"Max = %d, Avg = %d.\n",max,average);
        
        for(int i =0;i<H;i++){
        	int currIndex = keyPos[i];
        	fprintf(writeFile,"Hi I'm process %d with return arg 0 and I found a hidden key in position A[%d].\n",pid,currIndex);
        }
        fclose(writeFile);
        fclose(readFile);

    return 0;
}