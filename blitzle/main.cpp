#include <iostream>
#include <conio.h>
#include <opencv2/highgui.hpp>

#include "BlitzleApp.h"

int main(int argc, char** argv)
{
#ifdef _DEBUG
	std::cout << "Waiting for debug attach..." << std::endl;
	while (!_kbhit()) { }
#endif

	BlitzleApp blitzle(argc, argv);

	std::string mainWindow = std::string("Blitzle: ") + blitzle.getType();
	cv::namedWindow(mainWindow, cv::WINDOW_AUTOSIZE);

	int keyPressed = 0;
	while (keyPressed != 0x1B) // Exit on 'Esc'
	{
		cv::Mat result;
		blitzle.drawNextFrame(result);
		cv::imshow(mainWindow, result);
		keyPressed = cv::waitKey(1);
	}

	cv::destroyAllWindows();
	return 0;
}

