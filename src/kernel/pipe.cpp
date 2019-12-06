#pragma once

#include "pipe.h"

Pipe::Pipe(int p_buffer_size) {
	buffer_size = p_buffer_size;

	producer = new Semaphore(p_buffer_size);
	consumer = new Semaphore(0);

	closed_out = false;
	closed_in = false;
}

void Pipe::Close(Pipe_Function function) {

	switch (function) {
		case Pipe_Function::Write:
			closed_out = true;
			consumer->V();
			break;
		case Pipe_Function::Read:
			closed_in = true;
			producer->V();
			break;
	}
}

size_t Pipe::Produce(char *buffer, size_t buffer_length) {
	size_t produced = 0;

	for (size_t i = 0; i < buffer_length; i++) {

		if (closed_in) {
			return produced;
		}

		producer->P();
		std::unique_lock<std::mutex> locker(mutual_exclusion);

		// Insert into buffer.
		pipe_buffer.push_back(buffer[i]);

		locker.unlock();
		consumer->V();
		produced++;
	}

	return produced;
}

size_t Pipe::Consume(char *buffer, size_t buffer_length) {
	size_t consumed = 0;
	
	for (size_t i = 0; i < buffer_length; i++) {
		
		if (closed_out && pipe_buffer.empty()) {
			return consumed;
		}
		
		consumer->P();

		std::unique_lock<std::mutex> locker(mutual_exclusion);

		// Read from buffer.
		buffer[i] = pipe_buffer.front();
		pipe_buffer.pop_front();

		locker.unlock();
		
		producer->V();
		consumed++;		
	}

	return consumed;
}