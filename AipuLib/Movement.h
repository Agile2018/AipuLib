#ifndef Movement_h
#define Movement_h

#define LIMIT_MOVEMENT	5
#define RESIZE_WIDTH	320
#define RESIZE_HEIGTH	240

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

class Movement
{
public:
	Movement();
	~Movement();
	bool SetImage(Mat image);
	bool IsThereMovement();

private:
	vector<Mat> images;
	Mat GetDifferenceBetweenImages(Mat img1, Mat img2, Mat img3);
	bool IsThereDifference(Mat imgDiff);

};


#endif // !Movement_h

