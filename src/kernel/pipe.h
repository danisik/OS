#pragma once

#include "semaphore.h"

enum class Pipe_Function {
	Read = 0,
	Write
};

class Pipe {
	public:
		Semaphore *producer; // píše, pipe write
		Semaphore *consumer; // ète, pipe read
		size_t buffer_size;
		int values_count;
		std::vector<char*> buffer;
		std::mutex mutual_exclusion;

		Pipe(size_t p_buffer_size);

		void Close(Pipe_Function function);
		size_t Produce(char *buffer, size_t buffer_length);
		size_t Consume(char *buffer, size_t buffer_length);
};