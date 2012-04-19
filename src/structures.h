#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

struct PanelInfo
{
    // Compensating angle derivation (in degree)
    double angle;
    // 
    Rect panelRect;
};

// Return features of image
PanelInfo getPanelInfo(const Mat &, const int=180, const int=80);
