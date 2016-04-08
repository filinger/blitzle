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

inline float euclideanDist(const cv::Point2f& a, const cv::Point2f& b)
{
	cv::Point2f diff = a - b;
	return cv::sqrt(diff.x * diff.x + diff.y * diff.y);
}
