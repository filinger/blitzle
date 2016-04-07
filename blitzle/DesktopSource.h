#pragma once

#include <opencv2/core.hpp>

#include "DesktopDuplication.h"

namespace cv
{
	class DesktopSource
	{
	public:
		DesktopSource(UINT adapter, UINT output, UINT timeout);
		virtual ~DesktopSource();

		bool acquireNextFrame(Mat& frameMat);

	private:
		DesktopDuplication duplication;
	};
}
