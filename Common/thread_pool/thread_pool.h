#pragma once


#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <memory>
#include <functional>
#include <atomic>
#include <future>

#include <stdexcept>

namespace netlib
{
	using ThreadTask = std::function<void(void)>;
	
	class ThreadPool
	{
	public:		
		//typedef std::function<void(void )> ThreadTask;

		ThreadPool(int threadNumber);

		~ThreadPool();

		//往任务队列里添加任务
		bool append(ThreadTask task, bool bPriority = false);

		ThreadTask get_one_task();

		//启动线程池
		bool start(void);

		//停止线程池
		bool stop(void);

		// 线程池是否在运行
		bool IsRunning();
		

		// 获取线程池实例
		static ThreadPool* GetInstance();

		static void Destory()
		{
			if (nullptr != s_pThreadPool)
			{
				delete s_pThreadPool;
				s_pThreadPool = nullptr;
			}
		};
public:

		static ThreadPool*		s_pThreadPool;
private:
		//线程所执行的工作函数
		void threadWork(void);

private:
		std::mutex m_mutex;                                        //互斥锁
		
		std::atomic< bool> m_bRunning;                              //线程池是否在运行
		int m_nThreadNumber;                                       //线程数

		std::condition_variable_any m_condition_empty;             //当任务队列为空时的条件变量
		std::queue<ThreadTask> m_taskList;                          //任务队列
		std::vector<std::shared_ptr<std::thread>> m_vecThread;     //用来保存线程对象指针
		//空闲线程数量
		std::atomic<int>  m_idlThrNum;

	};
}

#define GSThreadPool_AddFun(x)	netlib::ThreadPool::GetInstance()->append(x)

#define GSThreadPool	netlib::ThreadPool::GetInstance()




