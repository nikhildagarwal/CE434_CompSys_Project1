#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
*  Takes in one command line inputs --> L 
*  L = the amount of numbers we want to add to our file.
*  This program must be run first before any others.
*/
int main(int argc, char* argv[]) {

	int L = atoi(argv[1]);
	
	
    // Seed the random number generator with the current time
    srand(time(NULL));
    
    // Open the output file for writing
    FILE *f = fopen("numbers.txt", "w");
    
    // Generate L random positive integers and write them to the file
    int min = 1;
    int max = 1000;
    int numbers[L+50];
    for (int i = 0; i < L+50; i++) {
        int num = (rand()% (max)) + 1 ;
        numbers[i] = num;
        //fprintf(f, "%d\n", num);
    }
    
    for (int j=0; j<50;j++){
        int randomIndices=0;
        do{
            randomIndices = (rand()% (L + 50));
        }
        while(numbers[randomIndices]==-1);
        numbers[randomIndices] = -1;        
    }
    
    
    for(int i=0; i<L+50; i++){
        fprintf(f, "%d\n", numbers[i]);
    }
    
    int counter =0;
    for(int i = 0;i<L+50;i++){
    	if(numbers[i] == -1){
    	counter++;
    	}
    }
    
    printf("Number of Keys in this file: %d\n",counter);
    
    // Close the output file
    fclose(f);
    
    return 0;
}
