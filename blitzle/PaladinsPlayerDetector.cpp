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

void PaladinsPlayerDetector::init(char** argv)
{
	templ = imread(argv[1]);
	cvtColor(templ, templ, CV_BGR2BGRA); // Template is CV_8UC3 but it should match the frame which is CV_8UC4

	namedWindow(paladins::control_window, WINDOW_AUTOSIZE);
	createTrackbar("Threshold", paladins::control_window, &thresholdValue, 255, nopCallback, this);
}

void PaladinsPlayerDetector::destroy()
{
	destroyWindow(paladins::control_window);
}

void PaladinsPlayerDetector::processFrame(const Mat& frameIn, vector<Point>& playersOut)
{
}

void PaladinsPlayerDetector::processFrameDebug(const Mat& frameIn, Mat& drawingOut)
{
	Mat matched;
	matchTemplate(frameIn, templ, drawingOut, TM_CCOEFF_NORMED);
	//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	//threshold(matched, drawingOut, thresholdValue, 255, THRESH_BINARY);
}
