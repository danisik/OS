#pragma once
#include <mutex>
#include <condition_variable>

class Semaphore {
	public:
		Semaphore(int s_count);
		void notify(int tid);
		void wait(int tid);

		std::mutex mtx;
		std::condition_variable cv;
		int count;
};
