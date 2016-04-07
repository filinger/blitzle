#pragma once

#include <opencv2/core/types.hpp>

namespace cv
{
	struct Hsv {
		int h;
		int s;
		int v;

		Scalar asScalar() {
			return Scalar(h, s, v);
		}
	};

	struct HsvRange {
		Hsv from;
		Hsv to;
	};
}
