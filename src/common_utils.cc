#include "common_utils.h"

// XXX: Deprecated
// Remove one pixel noise
void cleanNoise(Mat &src, Mat &dst)
{
    // TODO: Remove all shit...
    if (!dst.data)
        src.copyTo(dst);
    for (int y=0; y < src.rows; y++)
        for (int x=0; x < src.cols; x++)
        {
            if (src.data[y * src.cols + x] == 255)
            {
                dst.data[y * src.cols + x] = 255;
                continue;
            }
            int count = 0;
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++)
                {
                    if (dx == 0 && dy == 0)
                        continue;
                    if (x + dx < 0 || x + dx >= src.cols)
                        continue;
                    if (y + dy < 0 || y + dy >= src.rows)
                        continue;
                    count += src.data[(y + dy) * src.cols + x + dx]?0:1;
                }
            dst.data[y * src.cols + x] = (count>=1)?0:255;
        }   
}

// Simply rotate source image to dst image
void rotateImage(const Mat& source, Mat &dst, double angle)
{
    // To make transparent mode we dst should have the same corners as a source image
    source.copyTo(dst);
    // Getting rotation point and matrix
    Point2f src_center(source.cols/2.0F, source.rows/2.0F);
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
    // Rotating image
    warpAffine(source, dst, rot_mat, source.size(), INTER_LINEAR, BORDER_TRANSPARENT);
}
