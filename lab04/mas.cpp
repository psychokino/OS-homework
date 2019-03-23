#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#ifdef __cplusplus
/* 对于 C++ 编译器，指定用 C 的语法编译 */
extern "C" {
#endif

/**
 * @file threadpool.h
 * @brief Threadpool Header File
 */

 /**
 * Increase this constants at your own risk
 * Large values might slow down your system
 */
#define MAX_THREADS 64
#define MAX_QUEUE 65536

/* 简化变量定义 */
typedef struct threadpool_t threadpool_t;

/* 定义错误码 */
typedef enum {
    threadpool_invalid        = -1,
    threadpool_lock_failure   = -2,
    threadpool_queue_full     = -3,
    threadpool_shutdown       = -4,
    threadpool_thread_failure = -5
} threadpool_error_t;

typedef enum {
    threadpool_graceful       = 1
} threadpool_destroy_flags_t;

/* 以下是线程池三个对外 API */

/**
 * @function threadpool_create
 * @brief Creates a threadpool_t object.
 * @param thread_count Number of worker threads.
 * @param queue_size   Size of the queue.
 * @param flags        Unused parameter.
 * @return a newly created thread pool or NULL
 */
/**
 * 创建线程池，有 thread_count 个线程，容纳 queue_size 个的任务队列，flags 参数没有使用
 */
threadpool_t *threadpool_create(int thread_count, int queue_size, int flags);

/**
 * @function threadpool_add
 * @brief add a new task in the queue of a thread pool
 * @param pool     Thread pool to which add the task.
 * @param function Pointer to the function that will perform the task.
 * @param argument Argument to be passed to the function.
 * @param flags    Unused parameter.
 * @return 0 if all goes well, negative values in case of error (@see
 * threadpool_error_t for codes).
 */
/**
 *  添加任务到线程池, pool 为线程池指针，routine 为函数指针， arg 为函数参数， flags 未使用
 */
int threadpool_add(threadpool_t *pool, void (*routine)(void *),
                   void *arg, int flags);

/**
 * @function threadpool_destroy
 * @brief Stops and destroys a thread pool.
 * @param pool  Thread pool to destroy.
 * @param flags Flags for shutdown
 *
 * Known values for flags are 0 (default) and threadpool_graceful in
 * which case the thread pool doesn't accept any new tasks but
 * processes all pending tasks before shutdown.
 */
/**
 * 销毁线程池，flags 可以用来指定关闭的方式
 */
int threadpool_destroy(threadpool_t *pool, int flags);

#ifdef __cplusplus
}
#endif

#endif /* _THREADPOOL_H_ */

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//#include "threadpool.h"

/**
 * 线程池关闭的方式
 */
typedef enum {
    immediate_shutdown = 1,
    graceful_shutdown  = 2
} threadpool_shutdown_t;

/**
 *  @struct threadpool_task
 *  @brief the work struct
 *
 *  @var function Pointer to the function that will perform the task.
 *  @var argument Argument to be passed to the function.
 */
/**
 * 线程池一个任务的定义
 */

typedef struct {
    void (*function)(void *);
    void *argument;
} threadpool_task_t;

/**
 *  @struct threadpool
 *  @brief The threadpool struct
 *
 *  @var notify       Condition variable to notify worker threads.
 *  @var threads      Array containing worker threads ID.
 *  @var thread_count Number of threads
 *  @var queue        Array containing the task queue.
 *  @var queue_size   Size of the task queue.
 *  @var head         Index of the first element.
 *  @var tail         Index of the next element.
 *  @var count        Number of pending tasks
 *  @var shutdown     Flag indicating if the pool is shutting down
 *  @var started      Number of started threads
 */
/**
 * 线程池的结构定义
 *  @var lock         用于内部工作的互斥锁
 *  @var notify       线程间通知的条件变量
 *  @var threads      线程数组，这里用指针来表示，数组名 = 首元素指针
 *  @var thread_count 线程数量
 *  @var queue        存储任务的数组，即任务队列
 *  @var queue_size   任务队列大小
 *  @var head         任务队列中首个任务位置（注：任务队列中所有任务都是未开始运行的）
 *  @var tail         任务队列中最后一个任务的下一个位置（注：队列以数组存储，head 和 tail 指示队列位置）
 *  @var count        任务队列里的任务数量，即等待运行的任务数
 *  @var shutdown     表示线程池是否关闭
 *  @var started      开始的线程数
 */
struct threadpool_t {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t *threads;
  threadpool_task_t *queue;
  int thread_count;
  int queue_size;
  int head;
  int tail;
  int count;
  int shutdown;
  int started;
};

/**
 * @function void *threadpool_thread(void *threadpool)
 * @brief the worker thread
 * @param threadpool the pool which own the thread
 */
/**
 * 线程池里每个线程在跑的函数
 * 声明 static 应该只为了使函数只在本文件内有效
 */
static void *threadpool_thread(void *threadpool);

int threadpool_free(threadpool_t *pool);

threadpool_t *threadpool_create(int thread_count, int queue_size, int flags)
{
    if(thread_count <= 0 || thread_count > MAX_THREADS || queue_size <= 0 || queue_size > MAX_QUEUE) {
        return NULL;
    }

    threadpool_t *pool;
    int i;

    /* 申请内存创建内存池对象 */
    if((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL) {
        goto err;
    }

    /* Initialize */
    pool->thread_count = 0;
    pool->queue_size = queue_size;
    pool->head = pool->tail = pool->count = 0;
    pool->shutdown = pool->started = 0;

    /* Allocate thread and task queue */
    /* 申请线程数组和任务队列所需的内存 */
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
    pool->queue = (threadpool_task_t *)malloc
        (sizeof(threadpool_task_t) * queue_size);

    /* Initialize mutex and conditional variable first */
    /* 初始化互斥锁和条件变量 */
    if((pthread_mutex_init(&(pool->lock), NULL) != 0) ||
       (pthread_cond_init(&(pool->notify), NULL) != 0) ||
       (pool->threads == NULL) ||
       (pool->queue == NULL)) {
        goto err;
    }

    /* Start worker threads */
    /* 创建指定数量的线程开始运行 */
    for(i = 0; i < thread_count; i++) {
        if(pthread_create(&(pool->threads[i]), NULL,
                          threadpool_thread, (void*)pool) != 0) {
            threadpool_destroy(pool, 0);
            return NULL;
        }
        pool->thread_count++;
        pool->started++;
    }

    return pool;

 err:
    if(pool) {
        threadpool_free(pool);
    }
    return NULL;
}

int threadpool_add(threadpool_t *pool, void (*function)(void *),
                   void *argument, int flags)
{
    int err = 0;
    int next;

    if(pool == NULL || function == NULL) {
        return threadpool_invalid;
    }

    /* 必须先取得互斥锁所有权 */
    if(pthread_mutex_lock(&(pool->lock)) != 0) {
        return threadpool_lock_failure;
    }

    /* 计算下一个可以存储 task 的位置 */
    next = pool->tail + 1;
    next = (next == pool->queue_size) ? 0 : next;

    do {
        /* Are we full ? */
        /* 检查是否任务队列满 */
        if(pool->count == pool->queue_size) {
            err = threadpool_queue_full;
            break;
        }

        /* Are we shutting down ? */
        /* 检查当前线程池状态是否关闭 */
        if(pool->shutdown) {
            err = threadpool_shutdown;
            break;
        }

        /* Add task to queue */
        /* 在 tail 的位置放置函数指针和参数，添加到任务队列 */
        pool->queue[pool->tail].function = function;
        pool->queue[pool->tail].argument = argument;
        /* 更新 tail 和 count */
        pool->tail = next;
        pool->count += 1;

        /* pthread_cond_broadcast */
        /*
         * 发出 signal,表示有 task 被添加进来了
         * 如果由因为任务队列空阻塞的线程，此时会有一个被唤醒
         * 如果没有则什么都不做
         */
        if(pthread_cond_signal(&(pool->notify)) != 0) {
            err = threadpool_lock_failure;
            break;
        }
        /*
         * 这里用的是 do { ... } while(0) 结构
         * 保证过程最多被执行一次，但在中间方便因为异常而跳出执行块
         */
    } while(0);

    /* 释放互斥锁资源 */
    if(pthread_mutex_unlock(&pool->lock) != 0) {
        err = threadpool_lock_failure;
    }

    return err;
}

int threadpool_destroy(threadpool_t *pool, int flags)
{
    int i, err = 0;

    if(pool == NULL) {
        return threadpool_invalid;
    }

    /* 取得互斥锁资源 */
    if(pthread_mutex_lock(&(pool->lock)) != 0) {
        return threadpool_lock_failure;
    }

    do {
        /* Already shutting down */
        /* 判断是否已在其他地方关闭 */
        if(pool->shutdown) {
            err = threadpool_shutdown;
            break;
        }

        /* 获取指定的关闭方式 */
        pool->shutdown = (flags & threadpool_graceful) ?
            graceful_shutdown : immediate_shutdown;

        /* Wake up all worker threads */
        /* 唤醒所有因条件变量阻塞的线程，并释放互斥锁 */
        if((pthread_cond_broadcast(&(pool->notify)) != 0) ||
           (pthread_mutex_unlock(&(pool->lock)) != 0)) {
            err = threadpool_lock_failure;
            break;
        }

        /* Join all worker thread */
        /* 等待所有线程结束 */
        for(i = 0; i < pool->thread_count; i++) {
            if(pthread_join(pool->threads[i], NULL) != 0) {
                err = threadpool_thread_failure;
            }
        }
        /* 同样是 do{...} while(0) 结构*/
    } while(0);

    /* Only if everything went well do we deallocate the pool */
    if(!err) {
        /* 释放内存资源 */
        threadpool_free(pool);
    }
    return err;
}

int threadpool_free(threadpool_t *pool)
{
    if(pool == NULL || pool->started > 0) {
        return -1;
    }

    /* Did we manage to allocate ? */
    /* 释放线程 任务队列 互斥锁 条件变量 线程池所占内存资源 */
    if(pool->threads) {
        free(pool->threads);
        free(pool->queue);

        /* Because we allocate pool->threads after initializing the
           mutex and condition variable, we're sure they're
           initialized. Let's lock the mutex just in case. */
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->notify));
    }
    free(pool);
    return 0;
}


static void *threadpool_thread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;

    for(;;) {
        /* Lock must be taken to wait on conditional variable */
        /* 取得互斥锁资源 */
        pthread_mutex_lock(&(pool->lock));

        /* Wait on condition variable, check for spurious wakeups.
           When returning from pthread_cond_wait(), we own the lock. */
        /* 用 while 是为了在唤醒时重新检查条件 */
        while((pool->count == 0) && (!pool->shutdown)) {
            /* 任务队列为空，且线程池没有关闭时阻塞在这里 */
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        /* 关闭的处理 */
        if((pool->shutdown == immediate_shutdown) ||
           ((pool->shutdown == graceful_shutdown) &&
            (pool->count == 0))) {
            break;
        }

        /* Grab our task */
        /* 取得任务队列的第一个任务 */
        task.function = pool->queue[pool->head].function;
        task.argument = pool->queue[pool->head].argument;
        /* 更新 head 和 count */
        pool->head += 1;
        pool->head = (pool->head == pool->queue_size) ? 0 : pool->head;
        pool->count -= 1;

        /* Unlock */
        /* 释放互斥锁 */
        pthread_mutex_unlock(&(pool->lock));

        /* Get to work */
        /* 开始运行任务 */
        (*(task.function))(task.argument);
        /* 这里一个任务运行结束 */
    }

    /* 线程将结束，更新运行线程数 */
    pool->started--;

    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return(NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
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
		
		while(Thread_Pool_Data->QueueTask.empty()){
			//lock when no jobs
		}
		
		pthread_mutex_lock(&Thread_Pool_Data->lock);

		//鎖set
		job.function = Thread_Pool_Data->QueueTask.front().function ;
		job.function_arg = Thread_Pool_Data->QueueTask.front().function_arg;
		Thread_Pool_Data->QueueTask.pop();
		pthread_mutex_unlock(&Thread_Pool_Data->lock);
		
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

void TestExit()
{
	cout<<endl<<"Execute to The End , Test ~~ "<<endl;
	exit(1);
}

int main(int argc ,char**  argv){
	
	
	
	ifstream ifs(argv[1]);
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
	threadpool_t *pool;
	
	for(int thread_in_the_pool = 1 ; thread_in_the_pool <= MAX_THREAD ; thread_in_the_pool++){
		
		
		pool = threadpool_create(thread_in_the_pool, 20, 0);
		ofstream ofs_MT(output_name);
		
		Var_QuickSort.arr = UnsortedArray;
		Var_QuickSort.first[1] = 0;
		Var_QuickSort.last[1] = (ArraySize-1);
		Var_QuickSort.single_thread = false;
		
		//ThreadCheck = pthread_create( (Thread + (1)*sizeof(pthread_t)), NULL, Partition_MT_1, &Var_QuickSort); ThreadCreateCheck(ThreadCheck);	
		
		//TaskAssign(pool, Partition_MT_1, &Var_QuickSort, 0);
		threadpool_add(pool, Partition_MT_2, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_3, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_4, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_5, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_6, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_7, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_8, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_9, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_10, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_11, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_12, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_13, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_14, &Var_QuickSort,0);
		threadpool_add(pool, Partition_MT_15, &Var_QuickSort,0);
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
		output_name[6] = thread_in_the_pool + 48 ;//cout<<"end"<<endl;
		//cout<<"end"<<endl;
		//cout<<"end"<<endl;
		threadpool_destroy(pool, 0);
		
		
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
