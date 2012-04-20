#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "common_utils.h"
#include "structures.h"

using namespace cv;
using namespace std;

const char* WIN_VIDEO = "Video";
const int V_DELAY = 33;

char path[100] = "";
int num=0;

void processPanelPart(Mat binaryFrame, const PanelInfo &panelInfo, const int linePixelCount=10, const int linePixelGap=3)
{
    // Searching for vertical lines
    // TODO: Remove Hough line search add simplier iterative search
    Mat panelPart = binaryFrame(panelInfo.panelRect), dst;
    // Convert color to draw lines on debug output
    cvtColor(panelPart, dst, CV_GRAY2BGR);
    vector<Vec4i> lines;
    vector<Vec4i> verticalLines;
    HoughLinesP(panelPart, lines, 1, CV_PI/2, linePixelCount, linePixelCount, linePixelGap);
    for(size_t i = 0; i < lines.size(); i++)
    {
        Vec4i &l = lines[i];
        if (l[0] != l[2])
        // Not vertical case:
            continue;
        verticalLines.push_back(l);
        // Debuging drawing
        line(dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
    }
    // Debaging show
    imshow(WIN_VIDEO, dst);
}

void processFrame(const Mat &src, const PanelInfo &panelInfo)
{
    Mat dst, src_gray, tmp;
    // To make a binary mask changing src color to gray
    cvtColor(src, src_gray, CV_BGR2GRAY);
    // Compensating angle derivation.
    rotateImage(src_gray, tmp, panelInfo.angle);
    // Using otsu threshold binarisation.
    adaptiveThreshold(tmp, dst, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 3, 3);
    processPanelPart(dst, panelInfo);
    //sprintf(path, "processed/step_2-rotation/frame_%.4d.png", num++);
    //imwrite(path, dst);
}

int main(int argc, char *argv[])
{
    string filename = (argc > 1)?argv[1]:"test_video.mp4";
    VideoCapture capture(filename);
    if (!capture.isOpened())
    {
        cout << "Could not open file " << filename << endl;
        return -1;
    }

    namedWindow(WIN_VIDEO, CV_WINDOW_AUTOSIZE);
    Mat frame;
    capture >> frame;
    PanelInfo panelInfo = getPanelInfo(frame);
    for (; !frame.empty() && waitKey(V_DELAY) != 27; capture >> frame)
        processFrame(frame, panelInfo);
    return 0;
}
