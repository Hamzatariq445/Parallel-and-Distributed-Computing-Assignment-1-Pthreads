//I21-0707													     Hamza Tariq
//PDC														    Assignment # 1

//--------------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sched.h>
#include <cmath>
//--------------------------------------------------------------------------------------------------------------------------------

#define MAX_NUMBERS 100000
#define NUM_THREADS 16

//-------------------------------------------------------Structures

typedef struct Node 
{
  int data;
  struct Node* next;
} 
Node;


typedef struct ThreadData
{
  int* numbers;
  int num;
  int thread_id;
  Node** head_ptr;
} 
ThreadData;

typedef struct Index 
{
  int start;
  int end;
} 
Index;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//--------------------------------------------------------Merge Sort Algorithm

Node* merge(Node* left, Node* right)
{
  if (!left) 
  {
      return right;
  }
  if (!right) 
  {
      return left;
  }

  Node* merged = NULL;

  if (left->data <= right->data)
  {
      merged = left;
      merged->next = merge(left->next, right);
  }
  else
  {
      merged = right;
      merged->next = merge(left, right->next);
  }

  return merged;
}

Node* mergeSort(Node* head)
{
  if (!head || !head->next) 
  {
      return head;
  }

  Node* left = head;
  Node* right = head->next;

  while (right && right->next)
  {
      left = left->next;
      right = right->next->next;
  }

  Node* middle = left->next;
  left->next = NULL;

  return merge(mergeSort(head), mergeSort(middle));
}

//=====================================================================================================================


//------------------------------------------------Making List For Parrallel 

void* addNumbersToListParallel(void* arg) 
{
  ThreadData* data = (ThreadData*)arg;

  int i = data->thread_id;

  while (i < data->num) 
  {
    Node* newNode = (Node*)malloc(sizeof(Node));

    if (!newNode)
    {

        fprintf(stderr, "New Memory allocation failed\n");
        exit(EXIT_FAILURE);

    }

    newNode->data = data->numbers[i];

    pthread_mutex_lock(&lock);
    newNode->next = *(data->head_ptr);
    *(data->head_ptr) = newNode;

    pthread_mutex_unlock(&lock);

    i += NUM_THREADS;

  }
  pthread_exit(NULL);
}


//-------------------------------------------------------Reading From File

void ReadNum(int* Numbers, int* num) 
{

  FILE* File_Name = fopen("random_numbers.txt", "r");

  if (!File_Name)
  {

   fprintf(stderr, "Error opening file\n");
   exit(EXIT_FAILURE);
  
  }

  int k = 0;
  
  while (fscanf(File_Name, "%d", &Numbers[k]) == 1 && k < *num)
  {
      k++;
  }

  *num = k;

  fclose(File_Name);
}

//------------------------------------------------------------Sorting the list 

void* sortList(void* arg)
{

  ThreadData* data = (ThreadData*)arg;

  Node* sorted = mergeSort(*(data->head_ptr));
  *(data->head_ptr) = sorted;

  pthread_exit(NULL);

}

Node* mergeSortedLists(Node* list1, Node* list2) 
{
  return merge(list1, list2);
  
}

//-------------------------------------------------------writing sorted array to file

void writeSortedNumbers(Node* head,const char* fileName)
{

  FILE* outputFile = fopen(fileName, "w");

  if (!outputFile)
  {
      fprintf(stderr, "Error opening output file\n");
      
      return;
  }

  while (head) 
  {
  
      fprintf(outputFile, "%d\n", head->data);
      head = head->next;
  }

  fclose(outputFile);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() 
{

  struct timeval start, end;

  int Numbers[MAX_NUMBERS];
  int num = MAX_NUMBERS;
  
// ---------------------------------------------------------Reading from file

  ReadNum(Numbers, &num);

  gettimeofday(&start, NULL);

//--------------------------------------------------------- Serial Implementation

  Node* serial_head = NULL;
  int i = 0;
  while (i < num)
  {
      Node* newNode = (Node*)malloc(sizeof(Node));
  
   if (!newNode) 
    {
    
       fprintf(stderr, "New Memory allocation failed\n");
       exit(EXIT_FAILURE);
      
   }     
   
   //                                                    converting array to list
   
      newNode->data = Numbers[i];
   
      newNode->next = serial_head;
      serial_head = newNode;

      i++;
  }
  serial_head = mergeSort(serial_head);

  gettimeofday(&end, NULL);
  
  printf("-------------------------------------------------------------\n");
  printf("------------------------- Serial ----------------------------\n\n");

  double serial_seconds = (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);

  printf("Serial Merge Sort Execution Time : %f seconds \n", serial_seconds);

  double serial_gflops = (100000 * log2(100000)) / (serial_seconds * 1e9);

  printf("Serial Merge Sort GFLOPS         : %f\n", serial_gflops);
  
  printf("\n");

  
//------------------------------------------------------- Parallel Implementation

  gettimeofday(&start, NULL);

  Node* head_pointers[NUM_THREADS] = { NULL };
  
  pthread_t threads[NUM_THREADS];
  
  ThreadData Data[NUM_THREADS];
  
  Index indexes[NUM_THREADS];

  //Calculating the Chunk Size
  int chunk = num / NUM_THREADS;
  
  for (int i = 0; i < NUM_THREADS; ++i)
  {
    indexes[i].start = i * chunk;
    indexes[i].end = ((i + 1) * chunk) - 1;
  }

  i = 0;
  while ( i < NUM_THREADS) 
  {
    Data[i].numbers = Numbers + indexes[i].start;
    Data[i].num = indexes[i].end - indexes[i].start + 1;
    Data[i].thread_id = i;
    Data[i].head_ptr = &head_pointers[i];
    i++;
  }
  
  //Setting Attributes for the Threads 

  pthread_attr_t attr;
  pthread_attr_init(&attr);

  for (int i = 0; i < NUM_THREADS; i++) 
  {
      //Making Threads For Parallel Execution
      if (pthread_create(&threads[i], &attr, addNumbersToListParallel, (void*)&Data[i]) != 0)
      {
          fprintf(stderr, "Error creating thread\n");
          exit(EXIT_FAILURE);
      }
  }
  
  
  //Waiting For the Threads To return
  i = 0;
  while ( i < NUM_THREADS )
  {
      pthread_join(threads[i], NULL);
      i++;
  }

  pthread_t merge_threads[NUM_THREADS];
  
  i = 0;
  
  //Sorting All The Lists Created
  while ( i < NUM_THREADS) 
  {
      if (pthread_create(&merge_threads[i], NULL, sortList, (void*)&Data[i]) != 0)
      {
          fprintf(stderr, "Error creating thread\n");
          exit(EXIT_FAILURE);
      }
      i++;
  }

  i = 0;

  while ( i < NUM_THREADS)
  {
      pthread_join(merge_threads[i], NULL);
      i++;
  }
  
  //Merging All The lists 

  Node* parallel_mergedList = mergeSortedLists(head_pointers[0], head_pointers[1]);

  i = 2;
  while( i < NUM_THREADS)
  {
      parallel_mergedList = mergeSortedLists(parallel_mergedList, head_pointers[i]);
      i++;
  }
  
  parallel_mergedList = serial_head;

  gettimeofday(&end, NULL);

//---------------------------------------------Calculating GFLOPS & Time

  printf("-------------------------------------------------------------\n");
  printf("---------------------- Parallel -----------------------------\n\n");
  
    double parallel_seconds = (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);

    printf("Parallel Merge Sort Execution Time : %f seconds \n", parallel_seconds);

    double parallel_gflops = (100000 * log2(100000)) / (parallel_seconds * 1e9);
    printf("Parallel Merge Sort GFLOPS         : %f\n", parallel_gflops);


//----------------------------------------------------Writing To File
    writeSortedNumbers(serial_head,"Sorted_List_Serial");
    writeSortedNumbers(parallel_mergedList,"Sorted_List_Parallel");

    return 0;
}
