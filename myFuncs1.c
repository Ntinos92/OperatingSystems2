#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include "myHeader1.h"


void simulation(hashPT *hashedPT, Disk *hardDisk, Queue* physMem, helpHash* helpHash, Results *Res, FILE* file1, int frameNum, int* lines, char ref[], char protBit[], int* fileFlag, int* maxFlag, int maxRefs, int qRefs, int fileLines) {

	int num = 0;
	while(EOF!=fscanf(file1,"%s %s",ref,protBit)) {
		
		(*lines)++;
		num++;
		long hexa;
		unsigned int p_hash;
		unsigned int hashSlot;
		unsigned int offset;
		int dirty = 0;

		hexa=strtol(ref,NULL,16);
		offset = hexa & 0xfff; //last 3 bits for offset (12bits)
		p_hash = (hexa >> 12 ) & 0xfffff; //5 bits for p#
		if(!strcmp("W",protBit)) //check protection bit for R,W and change dirty bit on entry
		{
			
			(Res->writes)++;
			dirty = 1;
		}
		else if(!strcmp("R",protBit))
		{
			(Res->reads)++;
			dirty = -1;
		}
		
		(Res->fileReferences)++;

		insert_value(&hashedPT, p_hash, frameNum, dirty);  //inserts node in hashe table

		hashSlot = ( (p_hash) % (2*(hashedPT->size)) + offset );

		LRU(physMem, helpHash, p_hash, hashSlot, Res, hardDisk, dirty);


		if(maxRefs == *lines)
		{
			*maxFlag = 1;
			break;
		}

	
		if(num == qRefs)		//Process swapping
		{
			num = 0;
			if((*fileFlag) == 1)
			{
				*fileFlag = 2;
			}
			else
			{
				*fileFlag = 1;
			}
			break;
		}

		if(fileLines == *lines || *maxFlag )		//Simulation reached max referrences or end of file
		{
			if(*fileFlag == 1)
			{
				*fileFlag = 2;
			}
			else
			{
				*fileFlag = 1;
			}
		}


	}

}


struct hashPT *create_hash(int size) {
 
struct hashPT *htable;
 
int i;

htable = (struct hashPT *) malloc(sizeof(struct hashPT) * size);
 
if (htable == NULL) {
 
printf("\n Out of Memory !!");
 
return NULL;
}

htable->size = size;

for (i = 0; i < size; i++)
	htable[i].entry = NULL;
 
return htable;
 
}
 
struct hashnode *create_hashnode(int val)
{
 
struct hashnode *tmp;
 
tmp = (struct hashnode *) malloc(sizeof(struct hashnode));
 
if (tmp == NULL)
	printf("\n System out of Memory");
 
tmp->next = NULL;
 
tmp->pHash = val;
tmp->valid = -1;
tmp->dirty = -1;

return tmp;
 
}
 
int insert_value(struct hashPT **htable, int val, int hsize, int dirtyBit)
{
 
int slot;
 
struct hashPT *htptr = (*htable);
 
struct hashnode *tmp;
 
if ((*htable) == NULL) {
 
printf("\n Hash Table is not created");
 
return 0;
 
}
 
slot = (val) % (2*hsize);
 
if (htptr[slot].entry == NULL) {
 
tmp = create_hashnode(val);
 
htptr[slot].entry = tmp;
tmp->dirty = dirtyBit;
 
}
 
    else {
 
tmp = create_hashnode(val);
 
tmp->next = htptr[slot].entry;
 
htptr[slot].entry = tmp;
tmp->dirty = dirtyBit;
 
}
 
return 1;
 
}

struct Disk* createDisk(int size)
{
	struct Disk *temp = (struct Disk *) malloc(sizeof(struct Disk) * size);

	temp->size = size;

	temp->entry = NULL;
	return temp;
}

void insertDisk(Disk* hardDisk, unsigned int pHash, unsigned int physAddress)
{
	struct disknode *new;
	
	new = malloc(sizeof(disknode));
	if ( new == NULL )
	{
		return;
	}
	new->pHash = pHash;
	new->physAddress = physAddress;
	new->next = hardDisk->entry;

	hardDisk->entry = new;
	hardDisk->size ++;

}

struct Results* initResults() {

	printf("segfault");
	struct Results *tmp;

	tmp = (struct Results*)malloc(sizeof(Results));
	if(tmp == NULL)
	{ printf("segfault");}
	
	tmp->pageFaults=0;
	tmp->writes=0;
	tmp->reads=0;
	tmp->fileReferences=0;
	tmp->frames=0;

	return tmp;
}
 
// A utility function to create a new Queue Node. The queue Node
// will store the given 'pageNumber'
QNode* newQNode( unsigned pageNumber )
{
    // Allocate memory and assign 'pageNumber'
    QNode* temp = (QNode *)malloc( sizeof( QNode ) );
    temp->pageNumber = pageNumber;
 
    // Initialize prev and next as NULL
    temp->prev = temp->next = NULL;

    temp->dirty = 0;
 
    return temp;
}
 
// A utility function to create an empty Queue.
// The queue can have at most 'numberOfFrames' nodes
Queue* createQueue( int numberOfFrames )
{
    Queue* queue = (Queue *)malloc( sizeof( Queue ) );
 
    // The queue is empty
    queue->count = 0;
    queue->front = queue->rear = NULL;
 
    // Number of frames that can be stored in memory
    queue->numberOfFrames = numberOfFrames;
 
    return queue;
}
 
// A utility function to create an empty Hash of given capacity
helpHash* createHash( int capacity )
{
    // Allocate memory for hash
    helpHash* hash = (helpHash *) malloc( sizeof( helpHash ) );
    hash->capacity = capacity;
 
    // Create an array of pointers for refering queue nodes
    hash->array = (QNode **) malloc( hash->capacity * sizeof( QNode* ) );
 
    // Initialize all hash entries as empty
    int i;
    for( i = 0; i < hash->capacity; ++i )
        hash->array[i] = NULL;
 
    return hash;
}
 
// A function to check if there is slot available in memory
int AreAllFramesFull( Queue* queue )
{
    return queue->count == queue->numberOfFrames;
}
 
// A utility function to check if queue is empty
int isQueueEmpty( Queue* queue )
{
    return queue->rear == NULL;
}
 
// A utility function to delete a frame from queue
void deQueue( Queue* queue )
{
    if( isQueueEmpty( queue ) )
        return;
 
    // If this is the only node in list, then change front
    if (queue->front == queue->rear)
        queue->front = NULL;
 
    // Change rear and remove the previous rear
    QNode* temp = queue->rear;
    queue->rear = queue->rear->prev;
 
    if (queue->rear)
        queue->rear->next = NULL;
 
    free( temp );
 
    // decrement the number of full frames by 1
    queue->count--;
}
 
// A function to add a page with given 'pageNumber' to both queue
// and hash
void Enqueue( Queue* queue, helpHash* hash,unsigned int pHash, unsigned pageNumber, Results* Res, Disk* hardDisk, int dirty )
{
    // If all frames are full, remove the page at the rear
    if ( AreAllFramesFull ( queue ) )
    {
        // remove page from hash
        hash->array[ queue->rear->pageNumber ] = NULL;
        deQueue( queue );
	if(dirty == 1)
	{
		insertDisk(hardDisk,pHash, pageNumber);
	}
    }
 
    // Create a new node with given page number,
    // And add the new node to the front of queue
    QNode* temp = newQNode( pageNumber );
    temp->next = queue->front;
 
    // If queue is empty, change both front and rear pointers
    if ( isQueueEmpty( queue ) )
        queue->rear = queue->front = temp;
    else  // Else change the front
    {
        queue->front->prev = temp;
        queue->front = temp;
    }
 
    // Add page entry to hash also
    hash->array[ pageNumber ] = temp;
 
    // increment number of full frames
    queue->count++;
}
 
// This function is called when a page with given 'pageNumber' is referenced
// from cache (or memory). There are two cases:
// 1. Frame is not there in memory, we bring it in memory and add to the front
//    of queue
// 2. Frame is there in memory, we move the frame to front of queue
void LRU( Queue* queue, helpHash* hash, unsigned int pHash, unsigned pageNumber, Results* Res, Disk* hardDisk, int dirty )
{
    QNode* reqPage = hash->array[ pageNumber ];
 
    // the page is not in cache, bring it
    if ( reqPage == NULL )
	{
	Res->pageFaults++;
        Enqueue( queue, hash, pHash, pageNumber, Res, hardDisk, dirty );
	}
 
    // page is there and not at front, change pointer
    else if (reqPage != queue->front)
    {
        // Unlink rquested page from its current location
        // in queue.
        reqPage->prev->next = reqPage->next;
        if (reqPage->next)
           reqPage->next->prev = reqPage->prev;
 
        // If the requested page is rear, then change rear
        // as this node will be moved to front
        if (reqPage == queue->rear)
        {
           queue->rear = reqPage->prev;
           queue->rear->next = NULL;
        }
 
        // Put the requested page before current front
        reqPage->next = queue->front;
        reqPage->prev = NULL;
 
        // Change prev of current front
        reqPage->next->prev = reqPage;
 
        // Change front to the requested page
        queue->front = reqPage;
    }
}

void freeEverything(hashPT *hashedPT, Disk* hardDisk, Queue* physMem, helpHash* helpHash, Results *Res, int frameNum)
{


	for(int i = 0; i < hashedPT->size; ++i )	//Free Hash Table
	{
        	hashedPT->entry[i].next = NULL;
	}
		free(hashedPT);

	for(int i = 0; i < hardDisk->size; ++i )	//Free Disk
	{
        	hardDisk->entry[i].next = NULL;
	}
		free(hardDisk);

	
	for(int i = 0; i < helpHash->capacity; ++i )	//Free helping Hash
	{
        	helpHash->array[i] = NULL;
	}
		free(helpHash);


	while(physMem->front)			//Free LRU queue
	{
		physMem->front = NULL;
	}
		physMem->rear = NULL;
		free(physMem);

	free(Res);			//Free Results

}
 

