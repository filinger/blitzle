#pragma once

#include <thread>
#include <mutex>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <iostream>

#include "deps\interception.h"

enum ScanCode
{
	SCANCODE_ESC = 0x01,
	SCANCODE_TAB = 0x0F
};

class Interceptor
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float> fsec;

public:
	Interceptor();
	virtual ~Interceptor();

	void setTargetDelta(int dx, int dy);
	void unsetTarget();
	bool isTargetSet();

private:
	void init();
	void destroy();
	void run();
	void handleToggle();
	void handleMouse();

	InterceptionContext context;
	InterceptionDevice device;
	InterceptionStroke stroke;

	std::thread thread;
	std::recursive_mutex mutex;

	fsec deltaTime;
	int targetDeltaX;
	int targetDeltaY;
	bool enabled = false;
	bool done = false;
};
