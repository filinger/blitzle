#include "DesktopSource.h"
#include "DotaPlayerDetector.h"
#include "PaladinsPlayerDetector.h"

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

#define DRAW_DEBUG
#define SCALE_DOWN_OUTPUT

int main(int argc, char** argv)
{
	namedWindow(result_window, WINDOW_AUTOSIZE);

	DesktopSource desktop(0, 1000);
	PaladinsPlayerDetector detector;

	detector.init(argv);
	while (true) {
		Mat frame;
		desktop.acquireNextFrame(frame);

#ifdef DRAW_DEBUG
		Mat drawing;
		detector.processFrameDebug(frame, drawing);
		drawing.copyTo(frame);
#else
		vector<Point> players;
		detector.processFrame(frame, players);
		drawPlayers(players, frame);
#endif

#ifdef SCALE_DOWN_OUTPUT
		resize(frame, frame, Size(), 0.5, 0.5, INTER_AREA);
#endif

		imshow(result_window, frame);
		waitKey(1);
	}
	detector.destroy();

	waitKey();
	destroyWindow(result_window);
	return 0;
}
