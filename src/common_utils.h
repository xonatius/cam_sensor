#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

void cleanNoise(Mat &src, Mat &dst);
void rotateImage(const Mat &, Mat &, double);
