#include "PaladinsPlayerDetector.h"

namespace paladins
{
	const char* control_window = "Paladins Control";
}

PaladinsPlayerDetector::PaladinsPlayerDetector()
{
}

PaladinsPlayerDetector::~PaladinsPlayerDetector()
{
}

void PaladinsPlayerDetector::init()
{
	namedWindow(paladins::control_window, WINDOW_AUTOSIZE);
}

void PaladinsPlayerDetector::destroy()
{
	destroyWindow(paladins::control_window);
}

void PaladinsPlayerDetector::processFrame(const Mat& frameIn, vector<Point>& playersOut)
{
}
