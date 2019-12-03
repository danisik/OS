#pragma once

#include "semaphore.h"

Semaphore::Semaphore(int s_value = 0) {
	value = s_value;
}


void Semaphore::P() {
	std::unique_lock<std::mutex> lock(mtx);
	
	value--;
	while (value < 0) {
		cv.wait(lock);
	}

}

void Semaphore::V() {
	std::unique_lock<std::mutex> lock(mtx);

	value++;
	if (value <= 0) {
		cv.notify_one();
	}
}
