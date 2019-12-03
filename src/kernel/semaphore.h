#pragma once

#include <mutex>
#include <condition_variable>

class Semaphore {
	public:
		Semaphore(int s_value);
		void P();
		void V();

		std::mutex mtx;
		std::condition_variable cv;
		int value;
};
