#include "PaladinsPlayerDetector.h"

namespace paladins
{
	const char* control_window = "Paladins Control";
	RNG rng(12345);
}

PaladinsPlayerDetector::PaladinsPlayerDetector()
{
}

PaladinsPlayerDetector::~PaladinsPlayerDetector()
{
}

void PaladinsPlayerDetector::init(int argc, char** argv, bool debugMode)
{
	char* templateOpt = getCmdOption(argv, argv + argc, "-template");
	if (templateOpt)
	{
		hpBarTemplate = imread(templateOpt);
		cvtColor(hpBarTemplate, hpBarTemplate, CV_BGR2BGRA); // Template is CV_8UC3 but it should match the frame which is CV_8UC4
	}
	else
	{
		cout << "Paladins detector requires '-template {path_to_hp_template.png}' option specified." << endl;
		exit(1);
	}

	if (debugMode) {
		addControls();
	}
}

void PaladinsPlayerDetector::destroy()
{
}

void PaladinsPlayerDetector::processFrame(const Mat& frameIn, vector<Point>& playersOut)
{
	vector<vector<Point>> contours;
	findHpBarContours(frameIn, contours);
	/*
	const Point offset = Point(25, 50);
	for (size_t i = 0; i < contours.size(); i++)
	{
		int area = int(contourArea(contours[i]) * contourAreaScale);
		if (area < minContourArea || area >= maxContourArea) {
			continue;
		}
		Point bar_begin = contours[i][0];
		playersOut.push_back(bar_begin + offset);
	}
	*/
}

void PaladinsPlayerDetector::processFrameDebug(const Mat& frameIn, Mat& drawingOut)
{
	vector<vector<Point>> contours;
	findHpBarContours(frameIn, contours);
	drawingOut = frameIn;
	for (size_t i = 0; i < contours.size(); i++)
	{
		int area = int(contourArea(contours[i]) * contourAreaScale);
		if (area < minContourArea || area >= maxContourArea) {
			continue;
		}
		Scalar color = Scalar(paladins::rng.uniform(0, 255), paladins::rng.uniform(0, 255), paladins::rng.uniform(0, 255));
		drawContours(drawingOut, contours, (int)i, color, 2);
	}
}

void PaladinsPlayerDetector::findHpBarContours(const Mat& frameIn, vector<vector<Point>>& contoursOut)
{
	Mat result;
	matchTemplate(frameIn, hpBarTemplate, result, TM_CCOEFF_NORMED);

	double normalizedThreshold = (double)thresholdValue / (double)thresholdMax;
	threshold(result, result, normalizedThreshold, 1.0L, THRESH_BINARY);

	dilateX = max(dilateX, 1);
	dilateY = max(dilateY, 1);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(dilateX, dilateY));
	dilate(result, result, kernel);

	result.convertTo(result, CV_8UC1, 255);

	vector<Vec4i> hierarchy;
	findContours(result, contoursOut, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
}

void PaladinsPlayerDetector::addControls()
{
	namedWindow(paladins::control_window, WINDOW_AUTOSIZE);
	createTrackbar("Threshold", paladins::control_window, &thresholdValue, thresholdMax, nopCallback, this);
	createTrackbar("Min Area", paladins::control_window, &minContourArea, contourAreaMax, nopCallback, this);
	createTrackbar("Max Area", paladins::control_window, &maxContourArea, contourAreaMax, nopCallback, this);
	createTrackbar("DilateX", paladins::control_window, &dilateX, dilateMax, nopCallback, this);
	createTrackbar("DilateY", paladins::control_window, &dilateY, dilateMax, nopCallback, this);
}
