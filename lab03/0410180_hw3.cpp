#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
//#include <sys/shm.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <sys/time.h>
#include <iomanip>
#include <cstdlib>

#include<semaphore.h>
#include<pthread.h>
using namespace std;

struct QuickSort_Data //Var_QuickSort
{
	vector<int> arr ;
	int first[16],last[16];
	bool single_thread;
	//ofstream& output ;
};
sem_t Single_Thread_signal;
sem_t Multi_Thread_signal;
sem_t TaskDone_signal;
sem_t MT_PARTITION_SIGNAL[16];
sem_t Thread_BubbleSort_Done;


void *BubbleSort(vector<int>& data, int left, int right)
{
	for (int i = right ; i > left; --i)
        for (int j = left ; j < i; ++j )
            if (data[j] >data[j + 1])
                swap(data[j], data[j + 1]);
	return NULL;
}

void MT_qks(vector<int>& data, int left, int right, int thread, QuickSort_Data *struct_data)
{
    if( left < right ){
			
			
			//struct QuickSort_Data *struct_data;
			//struct_data = (struct QuickSort_Data*) Var_QuickSort;
            int i = left;
            int j = right + 1 ;
            int pivot = data[left];
            do{
               do{++i;} while( i < right && data[i] < pivot ) ;
			   //cout<<"i : "<<i<<endl;
               do{--j;} while( data[j] > pivot ) ;
			   //cout<<"j : "<<j<<endl;
               if( i < j ) swap( data[i] , data[j] );
			   //cout<<"swap"<<endl;
            } while ( i < j );
            swap( data[left] , data[j] );
			if(thread < 2)
			{
				//cout<<" thread : "<<thread <<" ||  left : "<< left  <<" ||  j-1 : "<<j-1<<"  ||  right : "<<right<<endl;
				struct_data->first[2] = left ;
				struct_data->last[2] = j-1 ; 
				struct_data->first[3] = j+1 ;
				struct_data->last[3] = right ;
				
			}
			else if(thread > 1 && thread < 8)
			{
				//cout<<" thread : "<<thread <<" ||  left : "<< left <<" ||  j-1 : "<<j-1<<"  ||  right : "<<right<<endl;
				struct_data->first[thread*2] = left ;
				struct_data->last[thread*2] = j-1 ;
				struct_data->first[thread*2 + 1] = j+1 ;
				struct_data->last[thread*2 +1 ] = right ;
			}
			else
			{
				cerr<<"program logic error , plz check "<<endl;
				exit(1);
			}
				
			/*if(level < 4)
			{	
				
				qks(data,left,j-1,level+1);
				qks(data,j+1,right,level+1);
			}*/
			/*else
			{	
				cout<<"left : "<<left<<"  ||   cut pivot : "<<j+1<<"  ||    right : "<<right<<endl;
				BubbleSort(data,left,j-1);
				BubbleSort(data,j+1,right);
				
			}*/
        }
//cout<<"end"<<endl;
	//return NULL;
}


void SemaphoreCreateCheck(int check)
{
	if(check != 0)
	{
		cerr<<" Semaphore Can't be Created "<<endl;
		exit(1);
	}
	
}

void ThreadCreateCheck(int check)
{
	if(check != 0)
	{
		cerr<<" Thread Can't be Created "<<endl;
		exit(1);
	}
	
}

void *Partition_MT_0(void *Var_QuickSort);
void *Partition_MT_1(void *Var_QuickSort);
void *Partition_MT_2(void *Var_QuickSort);
void *Partition_MT_3(void *Var_QuickSort);
void *Partition_MT_4(void *Var_QuickSort);
void *Partition_MT_5(void *Var_QuickSort);
void *Partition_MT_6(void *Var_QuickSort);
void *Partition_MT_7(void *Var_QuickSort);
void *Partition_MT_8(void *Var_QuickSort);
void *Partition_MT_9(void *Var_QuickSort);
void *Partition_MT_10(void *Var_QuickSort);
void *Partition_MT_11(void *Var_QuickSort);
void *Partition_MT_12(void *Var_QuickSort);
void *Partition_MT_13(void *Var_QuickSort);
void *Partition_MT_14(void *Var_QuickSort);
void *Partition_MT_15(void *Var_QuickSort);

int main(int argc ,char**  argv){
	
	
	
	ifstream ifs(argv[1]);
	ofstream ofs_ST("output2.txt");
	
	pthread_t Thread[17];
	
	struct timeval start,end;
	vector<int> UnsortedArray;
	int ReadInteger;
	
	ifs >> ReadInteger;
	UnsortedArray.assign(ReadInteger,0);
	int ArraySize = static_cast<int>(UnsortedArray.size()) ;
	for(int i = 0 ; i < ArraySize ; i++ )
	{
		ifs >> ReadInteger ;
		UnsortedArray[i] = ReadInteger ;
	}
	
	struct QuickSort_Data Var_QuickSort;
	for(int i = 0 ; i < 15 ; i++){
		Var_QuickSort.first[i] = 0;
		Var_QuickSort.last[i] = (ArraySize-1);
	}
	Var_QuickSort.arr = UnsortedArray;
	Var_QuickSort.single_thread = true;
	
	//Var_QuickSort.output = ofs_ST;
	//cout<<UnsortedArray.size()<<endl;
	
	int ThreadCheck = 1;
	ThreadCheck = sem_init(&Single_Thread_signal,1,0);	SemaphoreCreateCheck(ThreadCheck);
	ThreadCheck = sem_init(&Multi_Thread_signal,1,0);	SemaphoreCreateCheck(ThreadCheck);
	ThreadCheck = sem_init(&TaskDone_signal,1,0);		SemaphoreCreateCheck(ThreadCheck);
	ThreadCheck = sem_init(&Thread_BubbleSort_Done,1,0);SemaphoreCreateCheck(ThreadCheck);
	for(int i = 1 ; i < 16 ; i++)
	{
		ThreadCheck = sem_init(&MT_PARTITION_SIGNAL[i],1,0);
		SemaphoreCreateCheck(ThreadCheck);
	}
	
	ThreadCheck = pthread_create( Thread , NULL, Partition_MT_0, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	
	
		
	
	gettimeofday(&start,0);
	sem_post(&Single_Thread_signal);
	sem_wait(&TaskDone_signal);
	
	
	gettimeofday(&end,0);
	int sec = end.tv_sec - start.tv_sec ;
	int usec = end.tv_usec - start.tv_usec ;

	cout<<"single thread elapsed time : "<<setw(10)<<sec*1+(usec/1000000.0)<<" second   "<<endl;
	//cout<<UnsortedArray.size()<<endl;
	for(int i=0;i < ArraySize; i++){
		//cout<<i<<" "<<endl;
		ofs_ST<<(Var_QuickSort.arr[i])<<" ";
	}
	ofs_ST.close();
	ofstream ofs_MT("output1.txt");
	
	Var_QuickSort.arr = UnsortedArray;
	Var_QuickSort.first[1] = 0;
	Var_QuickSort.last[1] = (ArraySize-1);
	Var_QuickSort.single_thread = false;
	
	ThreadCheck = pthread_create( (Thread + (1)*sizeof(pthread_t)), NULL, Partition_MT_1, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);	
	ThreadCheck = pthread_create( (Thread + (2)*sizeof(pthread_t)), NULL, Partition_MT_2, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (3)*sizeof(pthread_t)), NULL, Partition_MT_3, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (4)*sizeof(pthread_t)), NULL, Partition_MT_4, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (5)*sizeof(pthread_t)), NULL, Partition_MT_5, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (6)*sizeof(pthread_t)), NULL, Partition_MT_6, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (7)*sizeof(pthread_t)), NULL, Partition_MT_7, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (8)*sizeof(pthread_t)), NULL, Partition_MT_8, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (9)*sizeof(pthread_t)), NULL, Partition_MT_9, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (10)*sizeof(pthread_t)), NULL, Partition_MT_10, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (11)*sizeof(pthread_t)), NULL, Partition_MT_11, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (12)*sizeof(pthread_t)), NULL, Partition_MT_12, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (13)*sizeof(pthread_t)), NULL, Partition_MT_13, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (14)*sizeof(pthread_t)), NULL, Partition_MT_14, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	ThreadCheck = pthread_create( (Thread + (15)*sizeof(pthread_t)), NULL, Partition_MT_15, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	
	gettimeofday(&start,0);
	sem_post(&Multi_Thread_signal);
	sem_wait(&TaskDone_signal);
	
	
	gettimeofday(&end,0);
	sec = end.tv_sec - start.tv_sec ;
	usec = end.tv_usec - start.tv_usec ;
	cout<<"multiple thread elapsed time : "<<setw(10)<<sec*1+(usec/1000000.0)<<" second   "<<endl;
	//cout<<"size "<<ArraySize<<endl;
	
	for(int i=0;i < ArraySize; i++){
		//cout<<Var_QuickSort.arr[i]<<" ";
		ofs_MT<<(Var_QuickSort.arr[i])<<" ";
	}
	
	return 0;
	
}

void *Partition_MT_0(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	
	
	
	
	
	sem_wait(&Single_Thread_signal);
	//cout<<"123"<<endl;
	MT_qks( (quicksort_data->arr) , (quicksort_data->first[0]) , (quicksort_data->last[0]) , 0 , quicksort_data );
	//cout<<"MT0"<<endl;
	sem_post(&MT_PARTITION_SIGNAL[2]);
	sem_post(&MT_PARTITION_SIGNAL[3]);
	//cout<<"MT0.1"<<endl;
	Partition_MT_2(Var_QuickSort);
	//cout<<"MT0.2"<<endl;
	Partition_MT_3(Var_QuickSort);
	//cout<<"MT0.3"<<endl;
    
	
	for(int i = 0 ; i < 8 ; i++)
	{
		//cout<<"MT0.4"<<i<<endl;
		sem_wait(&Thread_BubbleSort_Done);
	}
	sem_post(&TaskDone_signal);
	return NULL;
}

void *Partition_MT_1(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	
	
	
	/*
	if(quicksort_data->single_thread == true){
		sem_wait(&Single_Thread_signal);
		MT_qks( (quicksort_data->arr) , (quicksort_data->first[0]) , (quicksort_data->last[0]) , 0 , &quicksort_data );
		Partition_MT_2(&Var_QuickSort);
		Partition_MT_3(&Var_QuickSort);
	}
	else if(quicksort_data->single_thread == false)
	{	*/
	sem_wait(&Multi_Thread_signal);
	MT_qks( (quicksort_data->arr) , (quicksort_data->first[1]) , (quicksort_data->last[1]) , 1 , quicksort_data);
	
	/*}
	else
	{
		cerr<<" Partition_MT_1 error , not correct 'single_thread' value "<<endl;
		exit(1);
	}*/
    
	sem_post(&MT_PARTITION_SIGNAL[2]);
	sem_post(&MT_PARTITION_SIGNAL[3]);
	for(int i = 0 ; i < 8 ; i++)sem_wait(&Thread_BubbleSort_Done);
	
	sem_post(&TaskDone_signal);
	return NULL;
}

void *Partition_MT_2(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[2]);
	
	
	//cout<<"bool : "<<quicksort_data->single_thread<<endl;
	MT_qks( (quicksort_data->arr) , (quicksort_data->first[2]) , (quicksort_data->last[2]) , 2 , quicksort_data);
	//cout<<"MT2"<<endl;
	
	sem_post(&MT_PARTITION_SIGNAL[4]);
	sem_post(&MT_PARTITION_SIGNAL[5]);
	
    if(quicksort_data->single_thread == true){
		//cout<<"MT2.1"<<endl;
		Partition_MT_4(Var_QuickSort);//cout<<"MT2.2"<<endl;
		Partition_MT_5(Var_QuickSort);
	}
	//cout<<"MT2.3"<<endl;
	
	return NULL;
}

void *Partition_MT_3(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[3]);
	
	
	
	MT_qks( (quicksort_data->arr) , (quicksort_data->first[3]) , (quicksort_data->last[3]) , 3 , quicksort_data);
	//cout<<"MT3"<<endl;
	sem_post(&MT_PARTITION_SIGNAL[6]);
	sem_post(&MT_PARTITION_SIGNAL[7]);
	if(quicksort_data->single_thread == true){
		Partition_MT_6(Var_QuickSort);
		Partition_MT_7(Var_QuickSort);
	}
    
	
	
	return NULL;
}

void *Partition_MT_4(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[4]);
	
	
	
	MT_qks( (quicksort_data->arr) , (quicksort_data->first[4]) , (quicksort_data->last[4]) , 4 , quicksort_data);
	//cout<<"MT4"<<endl;
	sem_post(&MT_PARTITION_SIGNAL[8]);
	sem_post(&MT_PARTITION_SIGNAL[9]);
	if(quicksort_data->single_thread == true){
		Partition_MT_8(Var_QuickSort);
		Partition_MT_9(Var_QuickSort);
	}
    
	
	return NULL;
}

void *Partition_MT_5(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[5]);
	
	
	
	MT_qks( (quicksort_data->arr) , (quicksort_data->first[5]) , (quicksort_data->last[5]) , 5 , quicksort_data);
	//cout<<"MT5"<<endl;
	sem_post(&MT_PARTITION_SIGNAL[10]);
	sem_post(&MT_PARTITION_SIGNAL[11]);
	if(quicksort_data->single_thread == true){
		Partition_MT_10(Var_QuickSort);
		Partition_MT_11(Var_QuickSort);
	}
    
	
	
	return NULL;
}

void *Partition_MT_6(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[6]);
	
	
	
	MT_qks( (quicksort_data->arr) , (quicksort_data->first[6]) , (quicksort_data->last[6]) , 6 , quicksort_data);
	//cout<<"MT6"<<endl;
	sem_post(&MT_PARTITION_SIGNAL[12]);
	sem_post(&MT_PARTITION_SIGNAL[13]);
	if(quicksort_data->single_thread == true){
		Partition_MT_12(Var_QuickSort);
		Partition_MT_13(Var_QuickSort);
	}
    
	
	return NULL;
}

void *Partition_MT_7(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[7]);
	
	
	
	MT_qks( (quicksort_data->arr) , (quicksort_data->first[7]) , (quicksort_data->last[7]) , 7 , quicksort_data);
	sem_post(&MT_PARTITION_SIGNAL[14]);
	sem_post(&MT_PARTITION_SIGNAL[15]);
	if(quicksort_data->single_thread == true){
		Partition_MT_14(Var_QuickSort);
		Partition_MT_15(Var_QuickSort);
	}
	
    
	
	return NULL;
}

void *Partition_MT_8(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[8]);
	
	//cout<<"MT8"<<endl;
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[8]) , (quicksort_data->last[8]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT8.1"<<endl;
	return NULL;
}

void *Partition_MT_9(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[9]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[9]) , (quicksort_data->last[9]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT9.1"<<endl;
	return NULL;
}

void *Partition_MT_10(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[10]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[10]) , (quicksort_data->last[10]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT10.1"<<endl;
	return NULL;
}

void *Partition_MT_11(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[11]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[11]) , (quicksort_data->last[11]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT11.1"<<endl;
	return NULL;
}

void *Partition_MT_12(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[12]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[12]) , (quicksort_data->last[12]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT12.1"<<endl;
	return NULL;
}

void *Partition_MT_13(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[13]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[13]) , (quicksort_data->last[13]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT13.1"<<endl;
	return NULL;
}

void *Partition_MT_14(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[14]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[14]) , (quicksort_data->last[14]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT14.1"<<endl;
	return NULL;
}

void *Partition_MT_15(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[15]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[15]) , (quicksort_data->last[15]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT15.1"<<endl;
	return NULL;
}
