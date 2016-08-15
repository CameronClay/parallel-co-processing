#pragma once
#include <thread>
#include <memory>

class ThreadPool
{
public:
	ThreadPool(uint32_t threadCount)
		:
		threads(new std::thread[threadCount]),
		threadCount(threadCount)
	{}

	template<class Function, typename... Args>
	void InitializeTheadIndex(Function function, uint32_t index, Args&&... args)
	{
		new(threads.get() + index) std::thread(function, std::forward<Args>(args)...);
	}

	template<class Function, typename... Args>
	void Initialize(Function function, Args&&... args)
	{
		for (uint32_t i = 0; i < threadCount; i++)
			InitializeTheadIndex(function, i, std::forward<Args>(args)...);
	}
	void Wait()
	{
		for (uint32_t i = 0; i < threadCount; i++)
			threads[i].join();
	}
	uint32_t ThreadCount() const
	{
		return threadCount;
	}
	ThreadPool(ThreadPool&& threadPool)
		:
		threads(std::move(threadPool.threads)),
		threadCount(threadPool.threadCount)
	{
		memset(&threadPool, 0, sizeof(ThreadPool));
	}
private:
	std::unique_ptr<std::thread[]> threads;
	const uint32_t threadCount;
};
