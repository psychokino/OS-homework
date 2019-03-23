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

#include <queue>
#include <functional>
#include <assert.h>
using namespace std;

#define MAX_THREAD  8
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

//test
int xxx = 0;
//void* Task_Pool();

class ThreadPool{
	
	vector<pthread_t> POSIX_THREAD;
	void *pool;
	static void *ExecutingThread(void* arg);
	
public :
	ThreadPool(){
		
		//pool = this ;
		pthread_mutex_init(&lock,NULL);
		
		
	}
	
	void ThreadCreate(int ThreadNum){
		
		
		Total_Thread= ThreadNum ;
		OperatingThread = 1;
		
		sem_init(&OperatingThreadLock,1,1);//one thread and + 1 preventing self lock
		sem_init(&mutexlock,1,1);
		////POSIX_THREAD = new(pthread_t)*[ThreadNum];
		POSIX_THREAD.assign(Total_Thread , 0);
		for(int i = 0 ; i < Total_Thread ; i++){
			pthread_create(&POSIX_THREAD[i],NULL,ThreadPool::ExecutingThread, this);
		}
		
		
	}
	void TaskAssign( ThreadPool* pool_addr, void (*function)(void *), void* arg);
	~ThreadPool(){
		pthread_mutex_destroy(&lock);
		sem_destroy(&OperatingThreadLock);
	};
	
	void SetOperatingThread(int Set);
	int Look_NowOperatingThread(void){
		return OperatingThread;
	}
	
private :

	pthread_mutex_t lock;
	sem_t OperatingThreadLock;
	sem_t mutexlock;
	struct TaskPackage{
		//using ExecuteTask = function<void()>;
		void (*function)(void* ); 
		void *function_arg;
	} TaskUnit;
	queue<TaskPackage> QueueTask;
	
	int Total_Thread;
	
	int OperatingThread;
	
};

void ThreadPool::TaskAssign( ThreadPool* pool_addr, void (*function)(void *), void* arg)
{
	
	TaskPackage job;
	job.function = function ;
	job.function_arg = arg;
	pool_addr->QueueTask.push(job);
	
}


void* ThreadPool::ExecutingThread(void* arg)
{
	TaskPackage job;
	ThreadPool *Thread_Pool_Data = static_cast<ThreadPool*>(arg);
	
	while(1)
	{
		
		//int* test;
		//test =new int(9991);
		//sem_getvalue(&Thread_Pool_Data->OperatingThreadLock , test);
		//cout<<(*test)<<endl;
		
		sem_wait(&Thread_Pool_Data->OperatingThreadLock);
		//pthread_mutex_lock(&Thread_Pool_Data->lock);
		sem_wait(&Thread_Pool_Data->mutexlock);
		//cout<<"execute thread"<<endl;
		while(Thread_Pool_Data->QueueTask.empty()){
			//lock when no jobs
			//cout<<Thread_Pool_Data->QueueTask.size()<<endl;
		}
		

		//鎖set
		
		job.function = Thread_Pool_Data->QueueTask.front().function ;
		job.function_arg = Thread_Pool_Data->QueueTask.front().function_arg;
		Thread_Pool_Data->QueueTask.pop();
		//pthread_mutex_unlock(&Thread_Pool_Data->lock);
		sem_post(&Thread_Pool_Data->mutexlock);
		(*(job.function))(job.function_arg);
		
		sem_post(&Thread_Pool_Data->OperatingThreadLock);
		//cout<<"front end xxx = "<<xxx<<endl;xxx++;
		//cout<<"thread done"<<endl;
		
	}
}

void ThreadPool::SetOperatingThread(int Set)
{
	pthread_mutex_lock(&lock);
	//OperatingThread = Set;
	
	while(1)
	{
		//int* test;
		//test =new int(9991);
		//sem_getvalue(&OperatingThreadLock , test);
		//cout<<Set<<" "<<OperatingThread<<endl;
		if(Set > OperatingThread){
			//cout<<*test<<endl;
			sem_post(&OperatingThreadLock);
			OperatingThread++;
			
		}			
		else break;
	}
	//sem_destroy(&OperatingThreadLock);
	//sem_init(&OperatingThreadLock,1,Set);
	//xxx = Set + 1 ;
	pthread_mutex_unlock(&lock);
}

void *BubbleSort(vector<int>& data, int left, int right)
{
	for (int i = right ; i > left; --i)
        for (int j = left ; j < i; ++j )
            if (data[j] >data[j + 1])
                swap(data[j], data[j + 1]);
	return NULL;
}

void MT_qks(vector<int>& data, int left, int right, int thread, QuickSort_Data *struct_data)
{	//cout<<"444"<<endl;
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

void Partition_MT_1(void *Var_QuickSort);
void Partition_MT_2(void *Var_QuickSort);
void Partition_MT_3(void *Var_QuickSort);
void Partition_MT_4(void *Var_QuickSort);
void Partition_MT_5(void *Var_QuickSort);
void Partition_MT_6(void *Var_QuickSort);
void Partition_MT_7(void *Var_QuickSort);
void Partition_MT_8(void *Var_QuickSort);
void Partition_MT_9(void *Var_QuickSort);
void Partition_MT_10(void *Var_QuickSort);
void Partition_MT_11(void *Var_QuickSort);
void Partition_MT_12(void *Var_QuickSort);
void Partition_MT_13(void *Var_QuickSort);
void Partition_MT_14(void *Var_QuickSort);
void Partition_MT_15(void *Var_QuickSort);

/*void TestExit()
{
	cout<<endl<<"Execute to The End , Test ~~ "<<endl;
	exit(1);
}*/

int main(int argc ,char**  argv){
	
	ifstream ifs(argv[1]);
	if(!ifs.is_open()){
		cerr<<"argv[1] can't be loaded , loading \"input.txt\" automatically ."<<endl;
		ifs.close();
		ifs.open("input.txt");
		if(!ifs.is_open()){
			cerr<<"program terminated caused by loading \"input.txt\" failed , please check the file again ."<<endl;
			exit(1);
		}
		
	}
	
	
	//ofstream ofs_ST("output2.txt");
	
	//pthread_t Thread[17];
	
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
	
	ThreadCheck = sem_init(&Multi_Thread_signal,1,0);	SemaphoreCreateCheck(ThreadCheck);
	ThreadCheck = sem_init(&TaskDone_signal,1,0);		SemaphoreCreateCheck(ThreadCheck);
	ThreadCheck = sem_init(&Thread_BubbleSort_Done,1,0);SemaphoreCreateCheck(ThreadCheck);
	for(int i = 1 ; i < 16 ; i++)
	{
		ThreadCheck = sem_init(&MT_PARTITION_SIGNAL[i],1,0);
		SemaphoreCreateCheck(ThreadCheck);
	}
	
	//ThreadCheck = pthread_create( Thread , NULL, Partition_MT_0, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);
	
	
		
	
	
	int sec = end.tv_sec - start.tv_sec ;
	int usec = end.tv_usec - start.tv_usec ;

	
	char output_name[] = "output1.txt" ;
	ThreadPool *pool;
	pool = new (ThreadPool)();
	//pthread_mutex_t threadlock;
	//pthread_mutex_init(&threadlock,NULL);
	
	for(int thread_in_the_pool = 1 ; thread_in_the_pool <= MAX_THREAD ; thread_in_the_pool++){
		//pthread_mutex_lock(&threadlock);
		if(thread_in_the_pool == 1)pool->ThreadCreate(MAX_THREAD);
		
		
		pool->SetOperatingThread(thread_in_the_pool);
		ofstream ofs_MT(output_name);
		
		Var_QuickSort.arr = UnsortedArray;
		Var_QuickSort.first[1] = 0;
		Var_QuickSort.last[1] = (ArraySize-1);
		Var_QuickSort.single_thread = false;
		
		//ThreadCheck = pthread_create( (Thread + (1)*sizeof(pthread_t)), NULL, Partition_MT_1, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);	
		
		//TaskAssign(pool, Partition_MT_1, &Var_QuickSort, 0);
		pool->TaskAssign(pool, Partition_MT_2, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_3, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_4, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_5, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_6, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_7, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_8, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_9, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_10, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_11, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_12, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_13, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_14, &Var_QuickSort);
		pool->TaskAssign(pool, Partition_MT_15, &Var_QuickSort);
		//pthread_mutex_unlock(&threadlock);
		gettimeofday(&start,0);
		
		
		sem_post(&Multi_Thread_signal);
		Partition_MT_1(&Var_QuickSort);
		sem_wait(&TaskDone_signal);
		//cout<<"123"<<endl;
		
		gettimeofday(&end,0);
		sec = end.tv_sec - start.tv_sec ;
		usec = end.tv_usec - start.tv_usec ;
		cout<<thread_in_the_pool<<" threads elapsed time : "<<setw(10)<<sec*1+(usec/1000000.0)<<" second   "<<endl;
		//cout<<"size "<<ArraySize<<endl;
		//TestExit();
		
		for(int i=0;i < ArraySize; i++){
			//cout<<Var_QuickSort.arr[i]<<" ";
			ofs_MT<<(Var_QuickSort.arr[i])<<" ";
		}
		//cout<<"end"<<endl;
		//if(!pool->QueueTask.empty())exit(1);
		ofs_MT.close();//cout<<"end"<<endl;
		output_name[6] = thread_in_the_pool + 48 + 1 ;// 48 = ascii -> 0 
		//cout<<"end"<<endl;
		//cout<<"end"<<endl;
		//cout<<"end"<<endl;
		//threadpool_destroy(pool, 0);
		
		
	}
	
	return 0;
	
}


void Partition_MT_1(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT1 exec"<<endl;
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
    //cout<<"MT1++"<<endl;
	
    
	sem_post(&MT_PARTITION_SIGNAL[2]);
	sem_post(&MT_PARTITION_SIGNAL[3]);
	//cout<<"MT1+++"<<endl;
	for(int i = 0 ; i < 8 ; i++)sem_wait(&Thread_BubbleSort_Done);
	//cout<<"MT1++++"<<endl;
	sem_post(&TaskDone_signal);
	////return NULL;
	//cout<<"MT1 done"<<endl;
}

void Partition_MT_2(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT2 exec"<<endl;
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
	//cout<<"MT2 done"<<endl;
	//return NULL;
}

void Partition_MT_3(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT3 exec"<<endl;
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
    
	
	//cout<<"MT3 done"<<endl;
	//return NULL;
}

void Partition_MT_4(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT4 exec"<<endl;
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
    
	//cout<<"MT4 done"<<endl;
	//return NULL;
}

void Partition_MT_5(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT5 exec"<<endl;
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
    
	
	//cout<<"MT5 done"<<endl;
	//return NULL;
}

void Partition_MT_6(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT6 exec"<<endl;
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
    
	//cout<<"MT6 done"<<endl;
	//return NULL;
}

void Partition_MT_7(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT7 exec"<<endl;
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[7]);
	
	
 //cout<<"MT7"<<endl;
	MT_qks( (quicksort_data->arr) , (quicksort_data->first[7]) , (quicksort_data->last[7]) , 7 , quicksort_data);
	sem_post(&MT_PARTITION_SIGNAL[14]);
	sem_post(&MT_PARTITION_SIGNAL[15]);
	if(quicksort_data->single_thread == true){
		Partition_MT_14(Var_QuickSort);
		Partition_MT_15(Var_QuickSort);
	}
	
    
	//cout<<"MT7 done"<<endl;
	//return NULL;
}

void Partition_MT_8(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT8 exec"<<endl;
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[8]);
	
 //cout<<"MT8"<<endl;
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[8]) , (quicksort_data->last[8]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT8.1"<<endl;
	//return NULL;
	//cout<<"MT8 done"<<endl;
}

void Partition_MT_9(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT9 exec"<<endl;
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[9]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[9]) , (quicksort_data->last[9]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT9.1"<<endl;
	//return NULL;
	//cout<<"MT9 done"<<endl;
}

void Partition_MT_10(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT10 exec"<<endl;
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[10]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[10]) , (quicksort_data->last[10]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT10.1"<<endl;
	//return NULL;
	//cout<<"MT10 done"<<endl;
}

void Partition_MT_11(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT11 exec"<<endl;
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[11]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[11]) , (quicksort_data->last[11]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT11.1"<<endl;
	//return NULL;
	//cout<<"MT11 done"<<endl;
}

void Partition_MT_12(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT12 exec"<<endl;
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[12]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[12]) , (quicksort_data->last[12]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT12.1"<<endl;
	//return NULL;
	//cout<<"MT12 done"<<endl;
}

void Partition_MT_13(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	
	//cout<<"MT13 exec"<<endl;
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[13]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[13]) , (quicksort_data->last[13]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT13.1"<<endl;
	//return NULL;
	//cout<<"MT13 done"<<endl;
}

void Partition_MT_14(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT14 exec"<<endl;
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[14]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[14]) , (quicksort_data->last[14]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT14.1"<<endl;
	//return NULL;
	//cout<<"MT14 done"<<endl;
}

void Partition_MT_15(void *Var_QuickSort)//(vector<int> data, int left, int right, ofstream& ofs)
{	//cout<<"MT15 exec"<<endl;
	struct QuickSort_Data *quicksort_data;
	quicksort_data = (struct QuickSort_Data *) Var_QuickSort;
	sem_wait(&MT_PARTITION_SIGNAL[15]);
	
	
	
	BubbleSort( (quicksort_data->arr) , (quicksort_data->first[15]) , (quicksort_data->last[15]) );
	
	
    
	sem_post(&Thread_BubbleSort_Done);
	//cout<<"MT15.1"<<endl;
	//return NULL;
	//cout<<"MT15 done"<<endl;
}
