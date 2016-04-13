#include "Interceptor.h"

#include <iostream>

Interceptor::Interceptor()
{
	init();
	thread = std::thread(&Interceptor::run, this);
}

Interceptor::~Interceptor()
{
	done = true;
	thread.join();
	destroy();
}

void Interceptor::setTargetDelta(int dx, int dy)
{
	targetDeltaX = dx;
	targetDeltaY = dy;
}

void Interceptor::unsetTarget()
{
	targetDeltaX = 0;
	targetDeltaY = 0;
}

bool Interceptor::isTargetSet() const
{
	return targetDeltaX != 0 && targetDeltaY != 0;
}

void Interceptor::init()
{
	context = interception_create_context();

	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_UP);
	interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_MOVE);
}

void Interceptor::destroy()
{
	interception_destroy_context(context);
}

void Interceptor::run()
{
	while (!done && interception_receive(context, device = interception_wait(context), static_cast<InterceptionStroke *>(&stroke), 1) > 0)
	{
		auto t0 = clock::now();

		handleToggle();
		handleMouse();

		auto t1 = clock::now();
		deltaTime = t1 - t0;
	}
	std::cout << "Interceptor: done! Exiting thread..." << std::endl;
}

void Interceptor::handleToggle()
{
	if (interception_is_keyboard(device))
	{
		interception_send(context, device, &stroke, 1);

		InterceptionKeyStroke& kstroke = *reinterpret_cast<InterceptionKeyStroke *>(&stroke);
		if (kstroke.code == SCANCODE_TAB)
		{
			enabled = !enabled;
			std::cout << "Interceptor: " << (enabled ? "on" : "off") << std::endl;
		}
	}
}

void Interceptor::handleMouse()
{
	if (interception_is_mouse(device))
	{
		InterceptionMouseStroke& mstroke = *reinterpret_cast<InterceptionMouseStroke *>(&stroke);

		if (enabled && isTargetSet() && (!mstroke.flags & INTERCEPTION_MOUSE_MOVE_ABSOLUTE))
		{
			std::cout << "Interceptor: modified: " << targetDeltaX << " : " << targetDeltaY << std::endl;
			mstroke.x = (mstroke.x / 2) + targetDeltaX;
			mstroke.y = (mstroke.y / 2) + targetDeltaY;
			unsetTarget();
		}

		interception_send(context, device, &stroke, 1);
	}
}

