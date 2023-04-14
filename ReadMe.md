In this repository are the C files for CE434 Computer Systems Project 1.
**Must be executed in a LINUX/UNIX environment**

We start this project with a randomly generated text file of 10,000 positive numbers. This file can be generated with fileGenerator.c. We then place 50 negative numbers of value -1 into the text file at random indices.

From here the goal of the project is to use processes and multi-threading to find the position index of every -1, the average of all numbers excluding -1, and the maximum number of the randomly generated file.

oneProcess.c uses only a single process to scan the file.

DFS.c however takes in a command line input (argv[]) for the number of processes that we want to create in total. Each process that is generated will scan a certain portion of the file while calculating sub-averages and local maximums and communicating these values to a single parent process using pipes. Each process will also communicate to the parent process when it has found a key in its designated section.

Each process will scan exactly 1/(# of processes) of the file.

Files of Interest: DFS.c, oneProcess.c, fileGenerator.c

	DFS.c can be found in /Processes/DFS/
	
	oneProcess.c can be found in /Processes/oneProcess/
	
	fileGenerator.c can be found in /Processes/
	

Instructions to run the project: -
	-To create an input of different sizes(L), compile and run the fileGenerator.c file by doing gcc fileGenerator.c -o fileGenerator
	
	-When the input file numbers.txt is created, compile and run DFS.c and BFS.c using gcc <filename.c> -o <exe> and ./<exe>
	
	-Use the input file numbers.txt to compile and run the signal programs as well.
	
