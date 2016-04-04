#include "DotaPlayerDetector.h"

namespace dota 
{
	const char* control_window = "Dota Control";
	RNG rng(12345);
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

void DotaPlayerDetector::controlsCallback(int, void* context)
{
	// Do nothing, we'll process updates on next frame acquisition.
}

void DotaPlayerDetector::init()
{
	namedWindow(dota::control_window, WINDOW_AUTOSIZE);
	addControls();
}

void DotaPlayerDetector::destroy()
{
	destroyWindow(dota::control_window);
}


void DotaPlayerDetector::processFrame(Mat& frame, Mat& output)
{
	Mat hsv;
	cvtColor(frame, hsv, COLOR_BGR2HSV);

	Mat filtered;
	inRange(hsv, hsvRange.from.asScalar(), hsvRange.to.asScalar(), filtered);

	Mat dilated;
	dilateRect(dilateSize, filtered, dilated);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dilated, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	drawPlayers(dilated.size(), contours, frame);
	resize(frame, output, Size(), 0.5, 0.5, INTER_AREA);
}

void DotaPlayerDetector::drawPlayers(const Size& size, const vector<vector<Point>>& contours_in, Mat& drawing_out) {
	const Point offset = Point(50, 80);
	const int radius = 50;

	for (size_t i = 0; i < contours_in.size(); i++)
	{
		Point bar_begin = contours_in[i][0];
		Scalar color = Scalar(dota::rng.uniform(0, 255), dota::rng.uniform(0, 255), dota::rng.uniform(0, 255));
		circle(drawing_out, bar_begin + offset, radius, color, 2);
	}
}

void DotaPlayerDetector::dilateRect(int dilate_size, const Mat& filtered_in, Mat& dilated_out) {
	int rect_size = dilate_size == 0 ? 1 : dilate_size;
	Mat rect_kernel = getStructuringElement(MORPH_RECT, Size(rect_size, 1));
	dilate(filtered_in, dilated_out, rect_kernel, Point(-1, -1));
}

void DotaPlayerDetector::addControls() {
	const int h_max = 180;
	const int sv_max = 255;
	const int dilate_max = 100;

	createTrackbar("H1", dota::control_window, &hsvRange.from.h, h_max, controlsCallback, this);
	createTrackbar("S1", dota::control_window, &hsvRange.from.s, sv_max, controlsCallback, this);
	createTrackbar("V1", dota::control_window, &hsvRange.from.v, sv_max, controlsCallback, this);
	createTrackbar("H2", dota::control_window, &hsvRange.to.h, h_max, controlsCallback, this);
	createTrackbar("S2", dota::control_window, &hsvRange.to.s, sv_max, controlsCallback, this);
	createTrackbar("V2", dota::control_window, &hsvRange.to.v, sv_max, controlsCallback, this);
	createTrackbar("Dilate Size", dota::control_window, &dilateSize, dilate_max, controlsCallback, this);
}
