#ifndef MATRIXOPERATIONS_H
#define MATRIXOPERATIONS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <uchar.h>

#include <boost/any.hpp>
//#include <boost/random.hpp>
#include <random>

#include <sys/time.h>

using namespace cv;
using namespace std;

class MatrixOperations
{


    public:

        struct MatrixOperationsParams
        {
            Mat &src;
            Rect roi;
            boost::any param1;
            boost::any param2;
        };

        typedef void (MatrixOperations::*matrixFunctionPointer)(MatrixOperationsParams*);

        uchar table[256];

        MatrixOperations();
        virtual ~MatrixOperations();

        Mat createLookupTable(int base, uchar* table);

        Mat KernelMat(float data[], int dimX, int dimY);
        Mat KernelMatBoxFilter(int dSize);

        void ScanImageROI(MatrixOperationsParams *params);
        void ScanImage(MatrixOperationsParams *params);

        void ApplyFilterToROI(MatrixOperationsParams *params);
        void BrightenPhoto(MatrixOperationsParams *params);
        void ApplyBrightenToROI(MatrixOperationsParams *params);

        void GrayScaleROI(MatrixOperationsParams *params);
        void ReverseROI(MatrixOperationsParams *params);
        void MirrorLeftHalf(MatrixOperationsParams *params);

        void ZoomROI(MatrixOperationsParams *params);
        void PixelateROI(MatrixOperationsParams *params);

        void BlurROI(MatrixOperationsParams *params);
        void GaussianBlurROI(MatrixOperationsParams *params);
        void MedianBlurROI(MatrixOperationsParams *params);
        void BilateralFilterROI(MatrixOperationsParams *params);

        void SuperimposeImage(MatrixOperationsParams *params);


    protected:

    private:

};

#endif // MATRIXOPERATIONS_H
