#include "DesktopSource.h"
#include "DotaPlayerDetector.h"

#include <opencv2/highgui.hpp>

using namespace cv;

RNG rng(12345);
char const* result_window = "Result";

void drawPlayers(vector<Point>& playersIn, Mat& drawingOut) {
	const int radius = 50;
	for (size_t i = 0; i < playersIn.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		circle(drawingOut, playersIn[i], radius, color, 2);
	}
}

int main1(int argc, char** argv)
{
	namedWindow(result_window, WINDOW_AUTOSIZE);

	DesktopSource desktop(0, 1000);
	DotaPlayerDetector detector;

	detector.init();
	while (true) {
		Mat frame;
		desktop.acquireNextFrame(frame);

		vector<Point> players;
		detector.processFrame(frame, players);

		drawPlayers(players, frame);
		resize(frame, frame, Size(), 0.5, 0.5, INTER_AREA);

		imshow(result_window, frame);
		waitKey(1);
	}
	detector.destroy();

	waitKey(0);
	destroyWindow(result_window);
	return 0;
}
