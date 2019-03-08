
#pragma once
#include <mutex>

template<typename T>
class HSingletonTemplatePtr
{
public:
	static T* Instance()
	{
		if (m_instance == nullptr)
		{
			_mutex.lock();
			if (m_instance == nullptr)
			{
				m_instance = new T;;
				_mutex.unlock();
				atexit(Destory);
			}

		}
		return m_instance;
	};

protected:

private:
	HSingletonTemplatePtr();
	~HSingletonTemplatePtr();
	HSingletonTemplatePtr(HSingletonTemplatePtr&);
	HSingletonTemplatePtr& operator = (HSingletonTemplatePtr&);
	static void Destory()
	{
		if (nullptr != m_instance)
		{
			delete m_instance;
			m_instance = nullptr;
		}
	};

private:
	static T*  m_instance;
	static std::mutex _mutex;
};

template<typename T> T*  HSingletonTemplatePtr<T>::m_instance = nullptr;

template<typename T> std::mutex HSingletonTemplatePtr<T>::_mutex;

//#define gblDownloadMgrGet HSingletonTemplatePtr<CDownloadManager>::Instance()
