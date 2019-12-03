#include "semaphore.h"

Semaphore::Semaphore(int s_count = 0) {
	count = s_count;
}

void Semaphore::notify(int tid) {
	std::unique_lock<std::mutex> lock(mtx);

	count++;
	cv.notify_one();
}

void Semaphore::wait(int tid) {
	std::unique_lock<std::mutex> lock(mtx);

	while (count == 0) {
		cv.wait(lock);
	}
	count--;
}