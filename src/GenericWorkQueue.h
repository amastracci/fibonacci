#ifndef GENERIC_WORK_QUEUE_H
#define GENERIC_WORK_QUEUE_H

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class GenericWorkQueue {
public:
	void run();
	void spawn();
	void insertJob(T);
private:
	bool hasJobs() { return !_queue.empty(); }
	void waitForJobs();
	virtual void processJob(T) = 0;

	bool				_running;
	std::thread			_thread;
	std::mutex			_queueEmptyLock;
	std::condition_variable		_queueEmptyCond;
	std::queue<T>			_queue;
};

template <typename T>
void
GenericWorkQueue<T>::run()
{

	for (;;) {
		waitForJobs();
		T t = std::move(_queue.front());
		_queue.pop();

		try {
			processJob(std::move(t));
		} catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
}

template <typename T>
void
GenericWorkQueue<T>::spawn()
{
	_running = true;
	_thread = std::thread(&GenericWorkQueue::run, this);
	_thread.detach();
}


template <typename T>
void
GenericWorkQueue<T>::insertJob(T t)
{
	bool notify = !hasJobs();

	_queue.push(std::move(t));

	_queueEmptyCond.notify_all();
}

template <typename T>
void
GenericWorkQueue<T>::waitForJobs()
{

	if (hasJobs())
		return;

	std::unique_lock<std::mutex> lk(_queueEmptyLock);
	while (!hasJobs())
		_queueEmptyCond.wait(lk);
}

#endif // GENERIC_WORK_QUEUE_H
