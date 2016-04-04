#include "DesktopSource.h"

#include <opencv2/highgui.hpp>

using namespace cv;

char const* result_window = "Result";

int main(int argc, char** argv)
{
	DesktopSource desktop(0, 1000);
	namedWindow(result_window, WINDOW_AUTOSIZE);

	while (true) {
		Mat desktopFrame;
		desktop.acquireNextFrame(desktopFrame);
		imshow(result_window, desktopFrame);
		waitKey(1);
	}

	waitKey(0);
	destroyWindow(result_window);
	return 0;
}
