#include "DesktopSource.h"
#include "DotaPlayerDetector.h"

#include <opencv2/highgui.hpp>

using namespace cv;

char const* result_window = "Result";

int main(int argc, char** argv)
{
	namedWindow(result_window, WINDOW_AUTOSIZE);

	DesktopSource desktop(0, 1000);
	DotaPlayerDetector detector;

	detector.init();
	while (true) {
		Mat frame;
		desktop.acquireNextFrame(frame);

		Mat processed;
		detector.processFrame(frame, processed);

		imshow(result_window, processed);
		waitKey(1);
	}
	detector.destroy();

	waitKey(0);
	destroyWindow(result_window);
	return 0;
}
