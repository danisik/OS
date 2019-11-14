#pragma once

enum class State {
	Runnable = 1,
	Running,
	Blocked,
	Exited
};