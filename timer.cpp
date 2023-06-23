#include "timer.h"
#include<chrono>
#include<iostream>

void timer::set_start()
{
	this->start = std::chrono::system_clock::now();
}

void timer::set_end()
{
	this->end = std::chrono::system_clock::now();
}

void timer::compute_duration()
{
	this->time_cost = std::chrono::duration_cast<std::chrono::nanoseconds>(this->end - this->start);
}

void timer::print_time_cost()
{
	std::cout << (this->time_cost.count()) * std::chrono::nanoseconds::period::num << "ns" << std::endl;
}


