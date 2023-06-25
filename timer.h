#pragma once
#ifndef TIMER_H
#define TIMER_H
#include<chrono>
class timer
{
public:
	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;
	std::chrono::nanoseconds time_cost;
	void set_start();
	void set_end();
	void compute_duration();
	void print_time_cost();
};

#endif

