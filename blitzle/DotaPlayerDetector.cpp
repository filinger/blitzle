#include "DotaPlayerDetector.h"

namespace dota 
{
	const char* control_window = "Dota Control";
}

DotaPlayerDetector::DotaPlayerDetector()
{
	hsvRange.from = { 6, 255, 240 };
	hsvRange.to = { 8, 255, 255 };
	dilateSize = 5;
}

DotaPlayerDetector::~DotaPlayerDetector()
{
}

void DotaPlayerDetector::init(int argc, char** argv, bool debugMode)
{
	if (debugMode) {
		namedWindow(dota::control_window, WINDOW_AUTOSIZE);
		addControls();
	}
}

void DotaPlayerDetector::destroy()
{
}


void DotaPlayerDetector::processFrame(const Mat& frameIn, vector<Point>& playersOut)
{
	Mat hsv;
	cvtColor(frameIn, hsv, COLOR_BGR2HSV);

	Mat filtered;
	inRange(hsv, hsvRange.from.asScalar(), hsvRange.to.asScalar(), filtered);

	Mat dilated;
	dilateRect(dilateSize, filtered, dilated);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dilated, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	showPlayers(contours, playersOut);
}

void DotaPlayerDetector::processFrameDebug(const Mat& frameIn, Mat& drawingOut)
{
	// We could've actually draw some contours for debugging here....
	frameIn.copyTo(drawingOut);
}

void DotaPlayerDetector::showPlayers(const vector<vector<Point>>& contoursIn, vector<Point>& playersOut) {
	const Point offset = Point(50, 80);

	for (size_t i = 0; i < contoursIn.size(); i++)
	{
		Point bar_begin = contoursIn[i][0];
		playersOut.push_back(bar_begin + offset);
	}
}

void DotaPlayerDetector::dilateRect(int dilateSize, const Mat& filteredIn, Mat& dilatedOut) {
	int rect_size = dilateSize == 0 ? 1 : dilateSize;
	Mat rect_kernel = getStructuringElement(MORPH_RECT, Size(rect_size, 1));
	dilate(filteredIn, dilatedOut, rect_kernel, Point(-1, -1));
}

void DotaPlayerDetector::addControls() {
	const int h_max = 180;
	const int sv_max = 255;
	const int dilate_max = 100;

	createTrackbar("H1", dota::control_window, &hsvRange.from.h, h_max, nopCallback, this);
	createTrackbar("S1", dota::control_window, &hsvRange.from.s, sv_max, nopCallback, this);
	createTrackbar("V1", dota::control_window, &hsvRange.from.v, sv_max, nopCallback, this);
	createTrackbar("H2", dota::control_window, &hsvRange.to.h, h_max, nopCallback, this);
	createTrackbar("S2", dota::control_window, &hsvRange.to.s, sv_max, nopCallback, this);
	createTrackbar("V2", dota::control_window, &hsvRange.to.v, sv_max, nopCallback, this);
	createTrackbar("Dilate Size", dota::control_window, &dilateSize, dilate_max, nopCallback, this);
}
