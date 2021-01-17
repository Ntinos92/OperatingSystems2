#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include "myHeader1.h"

int main( int argc, char* argv[] )
{
	int k,ch,linesFile2=0,linesFile1=0;
	int frameNum, qRefs, maxRefs; //frames file references and max references
	FILE* file1;
	FILE* file2;

	char protBit[2];
	char ref[9];
	int fileFlag = 1;
	int maxFlag = 0;
	int lines1 = 0;
	int lines2 = 0;


	for(k=1; k<argc; k++)
    	{
    		if(argv[k]!=NULL)
    		{
			if(!strcmp(argv[k], "-f"))
			{
				frameNum =  atoi(argv[k+1]);
				printf("Frame Number: %d\n",frameNum);
			}
			else if(!strcmp(argv[k], "-q"))
			{
				qRefs = atoi(argv[k+1]);
				printf("Reference Number: %d\n",qRefs);
			}
			else if(!strcmp(argv[k], "-m"))
			{
				maxRefs = atoi(argv[k+1]);
				printf("MAX references : %d\n",maxRefs);
			}
		}
		else
		{
			printf("Wrong Arguments\n");
		}
	}


	// Open bzip.trace file
        if((file1 = fopen("bzip.trace", "r")) == NULL)
        {
                perror("Error! opening bzip.trace");
                exit(1);
        }

	do
	{
		ch = fgetc(file1);
    		if(ch == '\n')
        	{
			linesFile1++;
		}
	} while (ch != EOF);
	fseek( file1, 0, SEEK_SET );
	printf("Number of lines in file is: %d\n",linesFile1);


	// Open gcc.trace file
        if((file2 = fopen("gcc.trace", "r")) == NULL)
        {
                perror("Error! opening gcc.trace");
                exit(1);
        }

	do
	{
		ch = fgetc(file2);
    		if(ch == '\n')
        	{
			linesFile2++;
		}
	} while (ch != EOF);
	fseek( file2, 0, SEEK_SET );
	printf("Number of lines in file is: %d\n",linesFile2);


	struct Results *Res;
	struct hashPT *hashedPT;
	struct Queue *physMem;
	struct helpHash *helpHashed;
	struct Disk *hardDisk;

	Res = initResults();
	hashedPT = create_hash(2*frameNum);
	physMem = createQueue(2*frameNum);
	helpHashed = createHash(5000);
	hardDisk = createDisk(2*frameNum);


	while(1)	//Simulation start
	{
		if(fileFlag==1)
		{
			simulation(hashedPT, hardDisk, physMem, helpHashed, Res, file1, frameNum, &lines1, ref, protBit, &fileFlag, &maxFlag, maxRefs, qRefs, linesFile1);

		}
		if(fileFlag==2)
		{
			simulation(hashedPT, hardDisk, physMem, helpHashed, Res, file2, frameNum, &lines2, ref, protBit, &fileFlag, &maxFlag, maxRefs, qRefs, linesFile2);

		}

		if( (linesFile1 == lines1 && linesFile2 == lines2) || maxFlag )
		{
			break;
		}

	}

	fclose(file1);
	fclose(file2);


	printf("\n-----Statistics-----\n");

	printf("Writes: %d\n",(Res->writes));
	printf("Reads: %d\n",(Res->reads));
	printf("Page_Faults: %d\n",(Res->pageFaults));
	printf("Total file references: %d\n",(Res->fileReferences));

	freeEverything(hashedPT, hardDisk, physMem, helpHashed, Res, frameNum);		//frees allocated memory

	printf("Freed allocated memory succesfully!");

	printf("\n----End of Program----\n");
	return 0;
}

