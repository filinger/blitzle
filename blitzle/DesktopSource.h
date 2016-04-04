#pragma once

#include <opencv2/core.hpp>

#include "DesktopDuplication.h"

namespace cv
{

	class DesktopSource
	{
	public:
		DesktopSource(UINT output, UINT timeout);
		~DesktopSource();

		bool acquireNextFrame(Mat& frameMat);

	private:
		DesktopDuplication duplication;
	};
}
