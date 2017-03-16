#pragma once
#include<Windows.h>

class MyTimer {
private:
	LARGE_INTEGER freq, start, end;

public:
	MyTimer() {
		QueryPerformanceFrequency(&freq);
	}
	void timerStart() {
		QueryPerformanceCounter(&start);
	}
	void timerStop() {
		QueryPerformanceCounter(&end);
	}
	double getTime() {	// µ¥Î»Îªs
		double costTime = ((end.QuadPart - start.QuadPart)*1.0 / freq.QuadPart);
		return costTime;
	}
};