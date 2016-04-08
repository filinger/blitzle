#include "DotaPlayerDetector.h"

namespace dota 
{
	const char* controlsWindow = "Filtering Controls";
}

DotaPlayerDetector::DotaPlayerDetector()
{
}

DotaPlayerDetector::~DotaPlayerDetector()
{
}

void DotaPlayerDetector::init(int argc, char** argv, bool withDebug, bool withControls)
{
	if (withControls) {
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
	for (const auto& contour : contoursIn)
	{
		playersOut.push_back(contour[0] + offset);
	}
}

void DotaPlayerDetector::dilateRect(int dilateSize, const Mat& filteredIn, Mat& dilatedOut) {
	int kernelSize = dilateSize == 0 ? 1 : dilateSize;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(kernelSize, 1));
	dilate(filteredIn, dilatedOut, kernel);
}

void DotaPlayerDetector::addControls() {
	const int hMax = 180;
	const int svMax = 255;
	const int dilateMax = 100;

	namedWindow(dota::controlsWindow, WINDOW_AUTOSIZE);
	createTrackbar("H1", dota::controlsWindow, &hsvRange.from.h, hMax, nopCallback, this);
	createTrackbar("S1", dota::controlsWindow, &hsvRange.from.s, svMax, nopCallback, this);
	createTrackbar("V1", dota::controlsWindow, &hsvRange.from.v, svMax, nopCallback, this);
	createTrackbar("H2", dota::controlsWindow, &hsvRange.to.h, hMax, nopCallback, this);
	createTrackbar("S2", dota::controlsWindow, &hsvRange.to.s, svMax, nopCallback, this);
	createTrackbar("V2", dota::controlsWindow, &hsvRange.to.v, svMax, nopCallback, this);
	createTrackbar("Dilate Size", dota::controlsWindow, &dilateSize, dilateMax, nopCallback, this);
}
