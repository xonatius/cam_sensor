#include "structures.h"

#include <iostream>

#include "consts.h"
#include "common_utils.h"

using namespace std;

// Return features of image
PanelInfo getPanelInfo(const Mat &src, const int linePixelCount1, const int linePixelCount2)
{
    PanelInfo panelInfo;
    Mat srcGray, binImg;
    if (src.channels() != 1)
        cvtColor(src, srcGray, CV_BGR2GRAY);
    else
        srcGray = src;
    
    // Step 1: Finding derivation angle
    panelInfo.angle = 0.0;
    // Binarizing image
    adaptiveThreshold(srcGray, binImg, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 3, 3);
    // Line search
    vector<Vec2f> lines;
    HoughLines(binImg, lines, 1, CV_PI/180/4, linePixelCount1, 0, 0);
    // Finding mean of angles of all lines. (in radians)
    // TODO: Improvemnts:
    //          * Add weights
    //          * Remove "bad" lines
    //          * Check on zero lines
    for(size_t i=0; i < lines.size(); i++)
        panelInfo.angle += lines[i][1];
    panelInfo.angle /= lines.size();
    // Radians to degree and compensate
    panelInfo.angle = (panelInfo.angle - PI/2) * 180 / PI;
#ifdef DEBUG
    cout << "D: (getPanelInfo) angle: " << panelInfo.angle << endl;
#endif

    // Step 2: Finding operation rectangle
    // Rotating image
    Mat tmp;
    rotateImage(srcGray, tmp, panelInfo.angle);
    // Binarizing image
    adaptiveThreshold(tmp, binImg, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 3, 3);
    // Searching for all horizontal and vertical lines
#if 0
    vector<Vec4i> rectLines;
    HoughLinesP(binImg, rectLines, 1, CV_PI/2, linePixelCount2, linePixelCount2, 5);
#ifdef DEBUG
    for(size_t i=0; i < rectLines.size(); i++)
        cout << "(getPanelInfo) Rect line: " << rectLines[i][0] << " " << rectLines[i][1] 
             << " " << rectLines[i][2] << " " << rectLines[i][3] << endl;
#endif
#else
    lines.clear();
    HoughLines(binImg, lines, 1, CV_PI/2, linePixelCount2, 0, 0);
#ifdef DEBUG
    for(size_t i=0; i < lines.size(); i++)
    {
        float rho = lines[i][0], theta = lines[i][1];
        cout << "D: (getPanelInfo) Rect line: rho=" << rho << ", theta=" << theta << ", ";
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        cout << "x0=" << x0 << ", y0=" << y0 << endl;
    }
#endif
    vector<int> verticalLines, horizontalLines;
    // Parsing lines
    for(size_t i=0; i < lines.size(); i++)
    {
        float rho = lines[i][0], theta = lines[i][1];
        if (abs(theta - PI/2) < eps)
        // Horizontal
            horizontalLines.push_back(cvRound(sin(theta) * rho));
        else
        // Vertical
            verticalLines.push_back(cvRound(cos(theta) * rho));
    }
#ifdef WARNING
    if (verticalLines.size() > 2)
        cout << "W: (getPanelInfo) More then 2 vertical lines: " << verticalLines.size() << endl;
    if (horizontalLines.size() > 2)
        cout << "W: (getPanelInfo) More then 2 horizontal lines: " << horizontalLines.size() << endl;
#endif
    if (verticalLines.size() < 2)
        verticalLines.push_back(0);
    if (horizontalLines.size() < 2)
        horizontalLines.push_back(0);
    int left = verticalLines[0], 
        right = verticalLines[0], 
        top = horizontalLines[0], 
        bottom = horizontalLines[0];
    for (size_t i=1; i < verticalLines.size(); i++)
    {
        left = min(left, verticalLines[i]);
        right = max(right, verticalLines[i]);
    }
    for (size_t i=1; i < horizontalLines.size(); i++)
    {
        top = min(top, horizontalLines[i]);
        bottom = max(bottom, horizontalLines[i]);
    }
    panelInfo.panelRect = Rect(left, top, right - left, bottom - top);
#ifdef DEBUG
    cout << "D: (getPanelInfo) Rect: (x=" << panelInfo.panelRect.x << ", y=" << panelInfo.panelRect.y 
            << ", w=" << panelInfo.panelRect.width << ", h=" << panelInfo.panelRect.height << ")" << endl;
#endif
#endif
    return panelInfo;
}

