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


#define THREAD 3
#define QUEUE  30

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
using namespace std;


int tasks = 0, done = 0;
pthread_mutex_t lock;

int test(int doe){
	
	int x = 0,y=1;
	x = done + y ;
	return x ;
	
}

void dummy_task(void *arg) {
    
	
    pthread_mutex_lock(&lock);
    /* 记录成功完成的任务数 */
	cout<<done<<endl;	
    done++;
    pthread_mutex_unlock(&lock);
	test(done);
	sleep(2);
}



int main(int argc, char **argv)
{
    threadpool_t *pool;

    /* 初始化互斥锁 */
    pthread_mutex_init(&lock, NULL);

    /* 断言线程池创建成功 */
    assert((pool = threadpool_create(THREAD, QUEUE, 0)) != NULL);
    fprintf(stderr, "Pool started with %d threads and "
            "queue size of %d\n", THREAD, QUEUE);

    /* 只要任务队列还没满，就一直添加 */
    while(threadpool_add(pool, &dummy_task, NULL, 0) == 0) {
        pthread_mutex_lock(&lock);
        tasks++;
        pthread_mutex_unlock(&lock);
    }

    fprintf(stderr, "Added %d tasks\n", tasks);

    /* 不断检查任务数是否完成一半以上，没有则继续休眠 */
    while((tasks ) > done) {
        sleep(1);
    }
	//for(int i = 0 ; i < THREAD ; i++)pthread_join();
    /* 这时候销毁线程池,0 代表 immediate_shutdown */
    assert(threadpool_destroy(pool, 0) == 0);
    fprintf(stderr, "Did %d tasks\n", done);

    return 0;
}

/*#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace std
{
#define   MAX_THREAD_NUM 256

// 線程池,可以提交變參函數或拉姆達表達式的匿名函數執行,可以獲取執行返回值
// 不支持類成員函數,支持類靜態成員函數或全局函數,Opteron()函數等
class threadpool
{
	using Task = std::function< void ()> ;
	// 線程池
	std::vector<std::thread> pool;
	// 任務隊列
	std::queue<Task> tasks;
	// 同步
	std::mutex m_lock;
	// 條件阻塞
	std::condition_variable cv_task;
	// 是否關閉提交
	std::atomic<bool > stoped;
	// 空閒線程數量
	std::atomic< int >   idlThrNum;

public :
	inline threadpool(unsigned short size = 4 ) :stoped{ false }
	{
		idlThrNum = size < 1 ? 1 : size;
		for (size = 0 ; size < idlThrNum; ++ size)
		{    // 初始化線程數量
			pool.emplace_back(
				[ this ]
				{ // 工作線程函數
					while (! this ->stoped)
					{
						std::function< void ()> task;
						{    // 獲取一個待執行的task 
							std::unique_lock <std::mutex> lock { this ->m_lock }; // unique_lock相比lock_guard的好處是：可以隨時unlock()和lock() 
							this ->cv_task.wait(lock ,
								[ this ] {
									return  this ->stoped.load() || ! this -> tasks.empty();
								}
							); // wait直到有task 
							if ( this ->stoped && this -> tasks.empty())
								return ;
							task = std::move( this ->tasks.front()); // 取一個task 
							this ->tasks.pop();
						}
						idlThrNum-- ;
						task();
						idlThrNum++ ;
					}
				}
			);
		}
	}
	inline ~ threadpool()
	{
		stoped.store( true );
		cv_task .notify_all(); // 喚醒所有線程執行
		for(std::thread& thread : pool) {
			// thread.detach(); // 讓線程“自生自滅” 
			if (thread.joinable())
				thread.join(); // 等待任務結束，前提：線程一定會執行完
		}
	}

public :
	  // 提交一個任務
	  // 調用.get()獲取返回值會等待任務執行完,獲取返回值
	  // 有兩種方法可以實現調用類成員，
	  // 一種是使用bind： .commit(std::bind(&Dog::sayHello, &dog));
	  // 一種是用mem_fn： .commit(std::mem_fn(&Dog:: sayHello), &dog)
	template< class F, class ... Args>
	auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))>
	{
		if (stoped. load())     // stop == true ?? 
			throw std::runtime_error( " commit on ThreadPool is stopped. " );

		using RetType = decltype(f(args...)); // typename std: :result_of<F(Args...)>::type,函數f的返回值類型
		auto task = std::make_shared<std::packaged_task<RetType()> > (
		std::bind(std::forward<F>(f), std::forward<Args> (args)...)
		);     // wtf ! 
		std::future<RetType> future = task-> get_future();
		{     // 添加任務到隊列
			std::lock_guard<std::mutex> lock { m_lock }; // 對當前塊的語句加鎖lock_guard是mutex的stack封裝類，構造的時候lock( )，析構的時候unlock() 
			tasks.emplace(
				[task]()
				{ // push(Task{...}) 
					(* task)();
				}
			);
		}
		cv_task.notify_one(); // 喚醒一個線程執行

		return future;
	}

	// 空閒線程數量
	int idlCount() { return idlThrNum; }

	};

}



#include <iostream>

void fun1( int slp)
{
	printf( "   hello, fun1 ! %d\n " ,std::this_thread::get_id());
	if (slp> 0 ) {
		printf( " ======= fun1 sleep %d ========= %d\n " ,slp, std::this_thread::get_id());
		std::this_thread::sleep_for(std::chrono::milliseconds(slp));
	}
}

struct gfun{
	int  operator ()( int n) {
		printf( " %d hello, gfun ! %d\n " ,n, std::this_thread::get_id() );
		return  42 ;
	}
};

class A { 
public :
	static  int Afun( int n = 0 ) {    // 函數必須是static的才能直接使用線程池
		std::cout << n << "   hello, Afun !   "<< std::this_thread::get_id() << std::endl;
		return n;
	}

	static std:: string Bfun( int n, std:: string str, char c) {
		std:: cout << n << "   hello, Bfun !   " << str.c_str() << "   " << ( int )c << "   " << std::this_thread::get_id() << std::endl ;
		return str;
	}
};

int main()
	try {
		std::threadpool executor{ 8 };
		A a;
		std::future< void > ff = executor.commit(fun1, 0 );
		std::future< int > fg = executor .commit(gfun{}, 0 );
		std::future< int > gg = executor.commit(a.Afun, 9999 ); // IDE提示錯誤,但可以編譯運行
		std::future<std:: string > gh = executor.commit(A::Bfun, 9998 , " mult args" , 123 );
		std::future<std:: string > fh = executor.commit([]()->std:: string { std::cout << " hello, fh !   " << std:: this_thread::get_id() << std::endl; return  " hello,fh ret ! " ; });

		std::cout << " ======= sleep ======== = " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::microseconds( 900 ));

		for ( int i =0 ; i < 50 ; i++ ) {
			executor.commit(fun1,i* 100 );
		}
		std::cout << " ======= commit all ========= " << std::this_thread::get_id()<< " idlsize= " <<executor.idlCount() << std::endl;

		std::cout << " ======= sleep == ======= " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds( 3 ));

		ff. get (); // 調用.get()獲取返回值會等待線程執行完,獲取返回值
		std::cout << fg. get () << "   " << fh. get ().c_str ()<< "   " << std::this_thread::get_id() << std::endl;

		std::cout << " ======= sleep ========= " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds( 3 ));

		std::cout << "======= fun1,55 ========= "<< std::this_thread::get_id() << std::endl;
		executor.commit(fun1, 55 ). get ();     // 調用.get()獲取返回值會等待線程執行完

		std ::cout << " end... " << std::this_thread::get_id() << std::endl;


		std::threadpool pool( 4 );
		std::vector< std:: future< int > > results;

		for ( int i = 0 ; i < 8 ; ++ i) {
			results.emplace_back(
				pool.commit([i] {
					std::cout << " hello " << i << std::endl;
					std::this_thread::sleep_for(std::chrono::seconds( 1 ));
					std::cout << " world " << i << std::endl;
					return i* i;
				})
			);
		}
		std::cout << "======= commit all2 =========" << std::this_thread::get_id() << std::endl;

		for (auto && result : results)
			std::cout << result. get () << '  ' ;
		std:: cout << std::endl;
		return  0 ;
	}
	catch (std::exception& e) {
	std::cout << " some unhappy happened...   " << std::this_thread::get_id() << e.what() << std::endl;
	}
	*/




	