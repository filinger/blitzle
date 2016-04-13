#pragma once

#include <opencv2/core/types.hpp>

class DesktopSource;
class Interceptor;
class IPlayerDetector;

enum BlitzleDetector
{
	DOTA,
	PALADINS
};

struct BlitzleOpts
{
	BlitzleDetector detectorType = DOTA;
	bool withDebug = false;
	bool withControls = false;
	bool withCuda = false;
	float outputScale = 0.5f;
	float sensivity = 1.0f;
	int duplicationAdapter = 0;
	int duplicationOutput = 0;
	int duplicationTimeout = 2000;
};

class BlitzleApp
{
public:
	BlitzleApp(int argc, char** argv);
	virtual ~BlitzleApp();

	void drawNextFrame(cv::Mat& resultOut) const;
	void aimNextFrame() const;
	std::string getType() const;

private:
	static IPlayerDetector* instantiateDetector(BlitzleDetector detectorType);
	static BlitzleOpts parseCmdOptions(int argc, char** argv);
	static void drawPlayers(const std::vector<cv::Point>& playersIn, cv::Mat& drawingOut);
	static bool getClosestTarget(const std::vector<cv::Point>& playersIn, const cv::Point& originIn, cv::Point& targetOut);

	BlitzleOpts opts;
	IPlayerDetector* detector;
	DesktopSource* desktop;
	Interceptor* interceptor;
};
