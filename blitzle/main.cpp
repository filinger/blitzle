#include "DesktopSource.h"
#include "DotaPlayerDetector.h"
#include "PaladinsPlayerDetector.h"

#include <opencv2/highgui.hpp>

#include <iostream>
#include <string>
#include <algorithm>
#include <conio.h>

using namespace cv;

enum BlitzleDetector {
	DOTA, PALADINS
};

const std::string BlitzleDetectorStr[] = {
	"DOTA", "PALADINS"
};

struct BlitzleOpts {
	BlitzleDetector detectorType = DOTA;
	bool withDebug = false;
	bool withControls = false;
	float outputScale = 0.5f;
	int duplicationAdapter = 0;
	int duplicationOutput = 0;
	int duplicationTimeout = 2000;
};

IPlayerDetector* instantiateDetector(BlitzleDetector detectorType);
BlitzleOpts parseCmdOptions(int argc, char** argv);
void drawPlayers(vector<Point>& playersIn, Mat& drawingOut);

RNG rng(12345);

int main(int argc, char** argv)
{
#ifdef _DEBUG
	std::cout << "Waiting for debug attach..." << std::endl;
	while (!_kbhit()) { }
#endif
	
	BlitzleOpts opts = parseCmdOptions(argc, argv);
	DesktopSource desktop(opts.duplicationAdapter, opts.duplicationOutput, opts.duplicationTimeout);
	IPlayerDetector* detector = instantiateDetector(opts.detectorType);

	std::string mainWindow = std::string("Blitzle: ") + BlitzleDetectorStr[opts.detectorType];
	namedWindow(mainWindow, WINDOW_AUTOSIZE);

	detector->init(argc, argv, opts.withDebug, opts.withControls);

	int keyPressed = 0;
	while (keyPressed != 0x1B) // Exit on 'Esc'
	{
		Mat frame;
		desktop.acquireNextFrame(frame);

		Mat result = Mat(frame);
		if (opts.withDebug) 
		{
			detector->processFrameDebug(frame, result);
		}
		else 
		{
			vector<Point> players;
			detector->processFrame(frame, players);
			drawPlayers(players, result);
		}

		if (opts.outputScale != 1.0f) 
		{
			resize(result, result, Size(), opts.outputScale, opts.outputScale, INTER_AREA);
		}

		imshow(mainWindow, result);
		keyPressed = waitKey(1);
	}

	destroyAllWindows();
	detector->destroy();
	delete detector;
	return 0;
}

IPlayerDetector* instantiateDetector(BlitzleDetector detectorType)
{
	switch (detectorType) 
	{
	case PALADINS: return new PaladinsPlayerDetector();
	case DOTA: return new DotaPlayerDetector();
	default: return new DotaPlayerDetector();
	}
	return nullptr;
}

BlitzleOpts parseCmdOptions(int argc, char** argv)
{
	BlitzleOpts opts;
	char** end = argv + argc;

	if (cmdOptionExists(argv, end, "-dota"))
	{
		opts.detectorType = DOTA;
	}

	if (cmdOptionExists(argv, end, "-paladins"))
	{
		opts.detectorType = PALADINS;
	}

	if (cmdOptionExists(argv, end, "-debug"))
	{
		opts.withDebug = true;
	}

	if (cmdOptionExists(argv, end, "-controls"))
	{
		opts.withControls = true;
	}

	char* outputScaleOpt = getCmdOption(argv, end, "-scale");
	if (outputScaleOpt)
	{
		float outputScale = std::stof(std::string(outputScaleOpt));
		opts.outputScale = max(min(outputScale, 1.0f), 0.2f);
	}

	char* duplicationOutputOpt = getCmdOption(argv, end, "-output");
	if (duplicationOutputOpt)
	{
		int duplicationOutput = std::stoi(std::string(duplicationOutputOpt));
		opts.duplicationOutput = max(duplicationOutput, 0);
	}

	char* duplicationAdapterOpt = getCmdOption(argv, end, "-adapter");
	if (duplicationAdapterOpt)
	{
		int duplicationAdapter = std::stoi(std::string(duplicationAdapterOpt));
		opts.duplicationAdapter = max(duplicationAdapter, 0);
	}

	return opts;
}

void drawPlayers(vector<Point>& playersIn, Mat& drawingOut) 
{
	const int radius = 50;
	for (const auto& player : playersIn)
	{
		circle(drawingOut, player, radius, Scalar(255, 0, 255), 4);
	}
}
