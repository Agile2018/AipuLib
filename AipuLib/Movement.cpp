#include "Movement.h"

Movement::Movement()
{
}

Movement::~Movement()
{
}

bool Movement::SetImage(Mat image) {
	images.push_back(image);
	if (images.size() == 3)
	{
		return true;
	}
	return false;

}

Mat Movement::GetDifferenceBetweenImages(Mat img1, Mat img2, Mat img3) {
	Mat d1;
	Mat d2;
	Mat result;

	cv::absdiff(img3, img2, d1);
	cv::absdiff(img2, img1, d2);

	cv::bitwise_and(d1, d2, result);

	return result;

}

bool Movement::IsThereDifference(Mat imgDiff) {
	int cols = imgDiff.cols;
	int rows = imgDiff.rows;
	int dimension = cols * rows;

	int numberOfBlackPixels = 0;

	for (int i = 0; i < rows; i++)
	{
		const double* ptrRow = imgDiff.ptr<double>(i);
		for (int j = 0; j < cols; j++) {
			if (ptrRow[j] == 0.0)
			{
				numberOfBlackPixels++;
			}
		}

	}

	double avg = (numberOfBlackPixels * 100.0) / dimension;
	if (avg >= LIMIT_MOVEMENT)
	{
		return true;
	}
	return false;

}

bool Movement::IsThereMovement() {
	Mat i_minus;
	Mat i_m;
	Mat i_plus;

	cv::cvtColor(images[0], i_minus, cv::COLOR_RGB2GRAY);
	cv::cvtColor(images[1], i_m, cv::COLOR_RGB2GRAY);
	cv::cvtColor(images[2], i_plus, cv::COLOR_RGB2GRAY);
	cv::resize(i_minus, i_minus, cv::Size((RESIZE_WIDTH),
		(RESIZE_HEIGTH)), cv::INTER_CUBIC);
	cv::resize(i_m, i_m, cv::Size((RESIZE_WIDTH),
		(RESIZE_HEIGTH)), cv::INTER_CUBIC);
	cv::resize(i_plus, i_plus, cv::Size((RESIZE_WIDTH),
		(RESIZE_HEIGTH)), cv::INTER_CUBIC);
	Mat difference = GetDifferenceBetweenImages(i_minus, i_m, i_plus);
	bool result = IsThereDifference(difference);
	images.clear();
	return result;

}