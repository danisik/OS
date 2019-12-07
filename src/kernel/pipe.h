#pragma once

#include "semaphore.h"

#include <list>

enum class Pipe_Function {
	Read = 0,
	Write
};

class Pipe {
	public:
		Semaphore *producer; // píše, pipe write
		Semaphore *consumer; // ète, pipe read
		int buffer_size;
		std::list<char> pipe_buffer;
		std::mutex mutual_exclusion;
		
		bool closed_out;
		bool closed_in;

		Pipe(int p_buffer_size);

		void Close(Pipe_Function function);
		size_t Produce(char *buffer, size_t buffer_length);
		size_t Consume(char *buffer, size_t buffer_length);
};