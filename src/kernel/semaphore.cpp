#pragma once

#include "semaphore.h"

Semaphore::Semaphore(int s_value = 0)
{
	value = s_value;
	waiting = 0;
}


void Semaphore::P() 
{
	std::unique_lock<std::mutex> lock(mtx);
	
	if (value <= 0)
	{
		waiting++;
		cv.wait(lock);
		return;
	}
	value--;
}

void Semaphore::V() 
{
	std::unique_lock<std::mutex> lock(mtx);
	
	if (waiting > 0)
	{
		waiting--;
		cv.notify_one();
		return;
	}
	value++;
}
