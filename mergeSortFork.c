#define LEN 11
#define MAXEL 20
#define LEN_DEF 8
#define LEN_MAX 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


void print_list(int * list, int len)
{
	/*Print all the ints of a list in brackets followed by a newline*/
	int i;
	printf("[ ");
	for (i = 0; i < len; i++)
	{
		printf("%d ", list[i]);
	}
	printf("]\n\n");
}



void merge(int * list,int * llist,int lsize,int * rlist,int rsize){	

	int r = 0; //right_half index
	int l = 0; //left_half index
	int i = 0; //list index
 
	//merge left_half and right_half back into list
	for ( i =0, r = 0, l = 0; l < lsize && r < rsize; i++)
	{
		if ( llist[l] < rlist[r] ) { 
				list[i] = llist[l++]; 
			} else { 
				list[i] = rlist[r++]; 
			}
	}

	//Copy over leftovers of whichever temporary list hasn't finished
	for ( ; l < lsize; i++, l++) { 
		list[i] = llist[l];
	}

	for ( ; r < rsize; i++, r++) { 
		list[i] = rlist[r]; 
	}
}


void forking_mergesort(int * list,int n) {

	//termination condition
	if (n <= 1) {
		return; 
	}

	int lsize=(n/2);
	int rsize=(n/2+n%2);

	int i,r,l;
	int fd1[2],fd2[2],fd3[2],fd4[2];
	int pid1,pid2;
	int stat;

	//declare temporary lists
	int llist[lsize];
	int rlist[rsize];

	for (i = 0,l=0; i<lsize; i++, l++)
	{
		llist[l] = list[i];
	}

	for (i =lsize,r=0; i < n; i++, r++)
	{
		rlist[r] = list[i];
	}


	//pipes creating
	if(pipe(fd1) == -1){
		perror("Pipe error");
		exit(-1);
	}

	if(pipe(fd2) == -1){
		perror("Pipe error");
		exit(-1);
	}

	if(pipe(fd3) == -1){
		perror("Pipe error");
		exit(-1);
	}

	if(pipe(fd4) == -1){
		perror("Pipe error");
		exit(-1);
	}


	//fork using
	pid1=fork();
	switch(pid1){
		case -1://fork error
			perror("fork error\n");
			exit (-1);
		case 0://left_child process
			close(fd1[1]);
			close(fd2[0]);

			//read data from father
			while(read(fd1[0],&llist,sizeof(int)*(lsize)) !=0)

			//llist handling with fork_mergesort recursive calling
			forking_mergesort(llist,lsize);

			//return data of a left child to father
        	write(fd2[1],&llist, sizeof(int)*(lsize));
        	close(fd2[1]);

        	exit(0);
        default://father process
	        ////////////////////////////////////////////////right
        	close(fd1[0]);
        	close(fd2[1]);

        	//send data to the left child
        	write(fd1[1],&llist, sizeof(int)*(lsize));
        	close(fd1[1]);

        	//read data from left child
        	while(read(fd2[0],&llist,sizeof(int)*(lsize)) !=0)
			wait(NULL);
			close(fd2[0]);

	        	pid2=fork();
	        	switch(pid2){
	        		case -1://fork error
	        			perror("fork error\n");
	        			exit(-1);
	        		case 0://right child process
	        			close(fd3[1]);
	        			close(fd4[0]);

	        			//read data from father
	        			while(read(fd3[0],rlist,sizeof(int)*(rsize)) !=0)

	        			//llist handling with fork_mergesort recursive calling
						forking_mergesort(rlist,rsize);
	        		
	        			//return data of right child to father
	        			write(fd4[1],rlist,sizeof(int)*(rsize));	        			      	
	        			close(fd4[1]);
	        			exit(0);
	        		default:
	        		 	////////////////////////////////////////////////right
	        			close(fd3[0]);
	        			close(fd4[1]);
        		
	        			//send data to the right child
	        			write(fd3[1],&rlist,sizeof(int)*(rsize));
	        			close(fd3[1]);

	        			//read data from right child
						while(read(fd4[0],&rlist,sizeof(int)*(rsize)) !=0)
						wait(NULL);
						close(fd4[0]);

						//merge function calling
						merge(list,llist,lsize,rlist,rsize);
        		}
	}
}


//Main////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
int main(int argc, char* argv[]) {

	  char *fileName = NULL;
	  FILE * filePtr = NULL;
	  int i;
	  char valeur[LEN_MAX];

	  if(argc>1){

	  //Test if argv[1] =-f////////////////////////////////////////////////////////////////////////////////////
	  if(strcmp(argv[1],"-f")== 0){
	    int nbrow=0;
	    int tab[LEN_MAX];
	    fileName=argv[2];
	    filePtr = fopen(fileName, "rt");

	    if(filePtr == NULL) {
	      printf("File opening error\n");
	      exit(-1);
	    }

	    while (fgets(valeur,LEN_MAX, filePtr) != NULL){
	      //printf("%d \n", atoi(valeur));
	      tab[nbrow]=atoi(valeur);
	      nbrow++;
	    }
	   
	    //printing before sort
		printf("\nArray of integers generated from the data file %s\n",fileName);
		print_list(tab,nbrow);

		forking_mergesort(tab,nbrow);

		//printing after sort
		printf("Mergesort result:\n");
		print_list(tab,nbrow);
	  }

	//Test if argv[1]=-n///////////////////////////////////////////////////////////////////////////////////////
	if(argc<=2){
		printf("Parameter error: missing argument (number of elements) \n");
	}else{
			if(strcmp(argv[1],"-n")== 0){
		       	int nb=atoi(argv[2]);
		       	int tab[nb];

		       	time_t t;
		       	srand((unsigned) time(&t));

		        for(i=0; i<nb; i++){
			 		tab[i]=rand()%20;
		       	}
		 
		      	//printing before sort
				printf("\nArray of %d integers generated\n",nb);
				print_list(tab,nb);

				forking_mergesort(tab,nb);

				//printing after sort
				printf("Mergesort result:\n");
				print_list(tab,nb);
	   		}
	       
	    }


	 }

	//Test if argv [1] = null (default table of 11 ints is created)////////////////////////////////////////////
	if(argv[1]==NULL){
		int field[LEN];
		srand(time(NULL));

		// fill field with random values 0...30
		for (i=0; i<LEN; i++){ 
			field[i] = rand() % 30; 
		}

		//printing before sort
		printf("\nArray of integers generated by default\n");
		print_list(field,LEN);		
		forking_mergesort(field,LEN);
		//printing after sort
		printf("Mergesort result:\n");
		print_list(field,LEN);

	} 
	
return 0;

} 