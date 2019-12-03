#pragma once

#include "pipe.h"

Pipe::Pipe(size_t p_buffer_size) {
	buffer_size = p_buffer_size;
	values_count = 10;

	producer = new Semaphore(values_count);
	consumer = new Semaphore(0);
}

void Pipe::Close(Pipe_Function function) {
	
	switch (function) {
		case Pipe_Function::Read: 
			delete consumer;
			break;
		case Pipe_Function::Write:
			delete producer;
			break;
	}
}

size_t Pipe::Produce(char *buffer, size_t buffer_length) {
	producer->P();
	std::unique_lock<std::mutex> locker(mutual_exclusion);
	
	// Insert into buffer.
	
	locker.unlock();
	consumer->V();
	return 0;
}

size_t Pipe::Consume(char *buffer, size_t buffer_length) {
	consumer->P();
	std::unique_lock<std::mutex> locker(mutual_exclusion);
	
	// Read from buffer.
	
	locker.unlock();
	producer->V();
	return 0;
}