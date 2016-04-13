#pragma once

#include <opencv2/core/types.hpp>

namespace cv
{
	struct FilterValue
	{
		int h;
		int s;
		int v;

		Scalar asScalar() const
		{
			return Scalar(h, s, v);
		}
	};

	struct FilterRange
	{
		FilterValue from;
		FilterValue to;
	};

	inline float euclideanDist(const Point2f& a, const Point2f& b)
	{
		Point2f diff = a - b;
		return sqrt(diff.x * diff.x + diff.y * diff.y);
	}
}

