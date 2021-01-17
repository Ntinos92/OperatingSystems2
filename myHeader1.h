
typedef struct hashPT {
 
struct hashnode *entry;
int size;
 
}hashPT;


typedef struct hashnode {
 
unsigned int pHash;
int valid;
int dirty;
 
struct hashnode *next;
 
}hashnode;

 
typedef struct Disk{

int size;
struct disknode *entry;

}Disk;

typedef struct disknode {
 
unsigned int pHash;
unsigned int physAddress;
 
struct disknode *next;
 
}disknode;

typedef struct Results{

int pageFaults;
int writes;
int reads;
int fileReferences;
int frames;

}Results;

typedef struct QNode
{
    struct QNode *prev, *next;
    unsigned pageNumber;
    int dirty;
} QNode;

typedef struct Queue
{
    unsigned count;
    unsigned numberOfFrames;
    QNode *front, *rear;
} Queue;

typedef struct helpHash
{
    int capacity;
    QNode* *array;
} helpHash;


struct hashPT *create_hash(int size);

struct hashnode *create_hashnode(int val);

int insert_value(hashPT **htable, int val, int hsize, int dirtyBit);

void insertDisk(Disk* hardDisk, unsigned int pHash, unsigned int physAddress);

QNode* newQNode( unsigned pageNumber );
Queue* createQueue( int numberOfFrames );
helpHash* createHash( int capacity );
struct Disk* createDisk(int size);
int AreAllFramesFull( Queue* queue );
int isQueueEmpty( Queue* queue );
void deQueue( Queue* queue );
void Enqueue( Queue* queue, helpHash* hash,unsigned int pHash, unsigned pageNumber, Results* Res, Disk* hardDisk, int dirty );
void LRU( Queue* queue, helpHash* hash, unsigned int pHash, unsigned pageNumber, Results* Res, Disk* hardDisk, int dirty );

struct Results* initResults();

void simulation(hashPT *hashedPT, Disk* hardDisk, Queue* physMem, helpHash* helpHash, Results *Res, FILE* file1, int frameNum, int* lines, char ref[], char protBit[], int* fileFlag, int* maxFlag, int maxRefs, int qRefs, int fileLines);

void freeEverything(hashPT *hashedPT, Disk* hardDisk, Queue* physMem, helpHash* helpHash, Results *Res, int frameNum);
