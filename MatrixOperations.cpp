#include "MatrixOperations.h"

MatrixOperations::MatrixOperations()
{
    //ctor
}

MatrixOperations::~MatrixOperations()
{
    //dtor
}

void MatrixOperations::ApplyFilterToROI(MatrixOperationsParams *params)
{

    Mat processRegion, region, kernel;

    //grab region of src

    Rect rect = (params->roi);

    kernel = boost::any_cast<Mat>(params->param1);

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    //filter region

    filter2D( region, processRegion, region.depth(), kernel, Point ( -1, -1 ), 0, BORDER_CONSTANT  );

    //copy region back

    if (!processRegion.empty())
        processRegion.copyTo(region);

}

void MatrixOperations::ApplyBrightenToROI(MatrixOperationsParams *params)
{

    Mat region, temp;

    temp = params->src;

    Rect rect = (params->roi);

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    params->src = region;

    BrightenPhoto(params);

    params->src = temp;
}

void MatrixOperations::BrightenPhoto(MatrixOperationsParams *params)
{
    double alpha = boost::any_cast<double>(params->param1);
    double beta = boost::any_cast<double>(params->param2);

    for (int i =0; i<params->src.rows; i++)
    {
        for (int j =0; j<params->src.cols; j++)
        {
            for (int h = 0; h<params->src.channels()-1; h++)
            {
                params->src.at<Vec4b>(i,j)[h] = saturate_cast<uchar>(alpha * params->src.at<Vec4b>(i,j)[h] + beta);
            }
        }
    }
}


Mat MatrixOperations::createLookupTable(int base, uchar* table)
{
    for (int i = 0; i<256; i++)
    {
        table[i] = (uchar)(base * (i/base));
    }

    Mat lookupTable(1, 256, CV_8U);
    uchar* p = lookupTable.ptr();
    for( int i = 0; i<256; i++)
    {
        p[i] = table[i];
    }
    return lookupTable;
}

void MatrixOperations::ScanImageROI(MatrixOperationsParams *params)
{
    Mat &I = params->src;
    uchar *table = boost::any_cast<uchar *>(params->param1);
    CV_Assert(I.depth() == CV_8U);

    const int channels = I.channels();

    Rect rect = (params->roi);

    double x = rect.x;
    double y = rect.y;

    double yRange = y+rect.height;//y+height;
    double xRange = x+rect.width;////x+width;

    switch(channels)
    {
    case 1:
        {


            for (int i =x; i<xRange; i++)
            {
                for (int j =y; j<yRange; j++)
                {
                    uchar val = I.at<uchar>(j,i);
                    I.at<Vec3b>(j,i) = table[val];
                }
            }
            break;
        }
    case 4:
        {


            for (int i =x; i<xRange; i++)
            {
                for (int j =y; j<yRange; j++)
                {
                    Vec4b val = I.at<Vec4b>(j,i);
                    I.at<Vec4b>(j,i) [0] = table[val[0]];
                    I.at<Vec4b>(j,i) [1]  = table[val[1]];
                    I.at<Vec4b>(j,i) [2] = table[val[2]];

                }
            }
            break;
        }

    }

}

void MatrixOperations::ScanImage(MatrixOperationsParams *params)
{

    Mat &I = params->src;

    uchar *table = boost::any_cast<uchar *>(params->param1);

    CV_Assert(I.depth() == CV_8U);

    const int channels = I.channels();

    switch(channels)
    {
    case 1:
        {
            MatIterator_<uchar> it, _end;
            for( it = I.begin<uchar>(), _end = I.end<uchar>(); it != _end; ++it)
                *it = table[*it];
            break;
        }
    case 4:
        {
            MatIterator_<Vec4b> it, _end;

            for( it = I.begin<Vec4b>(), _end = I.end<Vec4b>(); it != _end; ++it)
            {
                (*it)[0] = table[(*it)[0]];
                (*it)[1] = table[(*it)[1]];
                (*it)[2] = table[(*it)[2]];
            }

            break;
        }

    }
}

void MatrixOperations::GrayScaleROI(MatrixOperationsParams *params)
{
    Mat region, gray_region, gray_region_4c;

    Rect rect = (params->roi);

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    //make gray region of 1 channel;
    cvtColor( region, gray_region, COLOR_BGR2GRAY );

    //make gray region of 4 channels
    cvtColor (gray_region, gray_region_4c, COLOR_GRAY2BGRA);

    if(!gray_region_4c.empty()) {
        gray_region_4c.copyTo(region);
    }
}

void MatrixOperations::ReverseROI(MatrixOperationsParams *params)
{
    Mat region;

    Rect rect = (params->roi);

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    Mat reverse_region(region.rows, region.cols, CV_8UC3, Scalar(0,0, 0));

    int column_size = region.cols;
    int row_size = region.rows;

    for(int i = 0; i<row_size; i++)
    {
        for (int j = 0; j<column_size; j++)
        {
            for (int h = 0; h<region.channels(); h++)
            {
                reverse_region.at<Vec4b>(i,column_size-j)[h] = region.at<Vec4b>(i,j)[h];
            }
        }
    }

    reverse_region.copyTo(region);

}

void MatrixOperations::MirrorLeftHalf(MatrixOperationsParams *params)
{

    Mat region;

    Rect rect = (params->roi);

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    Mat reverse_region(region.rows, region.cols, CV_8UC3, Scalar(0,0, 0));

    int column_size_half = region.cols/2.0;
    int column_size = region.cols;
    int row_size = region.rows;

    int padding = 2;

    for(int i = 0; i<row_size; i++)
    {
        for (int j = 0; j<column_size_half; j++)
        {
            for (int h = 0; h<region.channels(); h++)
            {
                reverse_region.at<Vec4b>(i,column_size-j)[h] = region.at<Vec4b>(i,j)[h];
            }
        }
    }

    for(int i = 0; i<row_size; i++)
    {
        for (int j = 0; j<column_size_half+padding; j++)
        {
            for (int h = 0; h<region.channels(); h++)
            {
                reverse_region.at<Vec4b>(i,j)[h] = region.at<Vec4b>(i,j)[h];
            }
        }
    }

    reverse_region.copyTo(region);

}

void MatrixOperations::ZoomROI(MatrixOperationsParams *params)
{
    Mat region, ZoomRegion, section;

    Rect rect = (params->roi);

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    pyrUp(region, ZoomRegion, Size(region.cols*2, region.rows*2));

    section = ZoomRegion(Rect(region.cols/2.0f, region.rows/2.0f, rect.width, rect.height));

    section.copyTo(region);
}

void MatrixOperations::PixelateROI(MatrixOperationsParams *params)
{
    static std::random_device seeder;
    static std::mt19937 rng(seeder());
    static std::uniform_real_distribution<double> gen(0.0, 255.0);

    struct timeval frame_begin = boost::any_cast<struct timeval>(params->param1);

    struct timeval frame_end;

    gettimeofday(&frame_end, NULL);

    long seconds = frame_end.tv_sec - frame_begin.tv_sec;

    long useconds = frame_end.tv_usec - frame_begin.tv_usec;

    long milli_time = ((seconds) * 1000 + useconds/1000.0);

    if (milli_time >= 200)
    {
        params->param1 = frame_end;
        params->param2 = gen(rng);
    }

    Mat &I = params->src;

    Rect rect = (params->roi);

    int value = boost::any_cast<double>(params->param2);

    double x = rect.x;
    double y = rect.y;

    double yRange = y+rect.height;//y+height;
    double xRange = x+rect.width;////x+width;


    for (int i =x; i<xRange; i++)
    {
        for (int j =y; j<yRange; j++)
        {
            I.at<Vec4b>(j,i) [2] = saturate_cast<uchar>(value);
        }
    }
}

void MatrixOperations::BilateralFilterROI(MatrixOperationsParams *params)
{
    Mat region, blurRegion;

    int kernelSize = boost::any_cast<int>(params->param1);

    Rect rect = params->roi;

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    bilateralFilter(region, blurRegion, kernelSize, kernelSize*2, kernelSize/2);

    blurRegion.copyTo(region);
}

void MatrixOperations::MedianBlurROI(MatrixOperationsParams *params)
{
    Mat region, blurRegion;

    int kernelSize = boost::any_cast<int>(params->param1);

    Rect rect = params->roi;

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    medianBlur(region, blurRegion, kernelSize);

    blurRegion.copyTo(region);
}

void MatrixOperations::GaussianBlurROI(MatrixOperationsParams *params)
{
    Mat region, blurRegion;

    int kernelSize = boost::any_cast<int>(params->param1);

    Rect rect = params->roi;

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    GaussianBlur(region, blurRegion, Size(kernelSize, kernelSize), 0, 0);

    blurRegion.copyTo(region);
}

void MatrixOperations::BlurROI(MatrixOperationsParams *params)
{
    Mat region, blurRegion;

    int kernelSize = boost::any_cast<int>(params->param1);

    Rect rect = params->roi;

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    blur(region, blurRegion, Size(kernelSize, kernelSize), Point(-1, -1));

    blurRegion.copyTo(region);
}

void MatrixOperations::SuperimposeImage(MatrixOperationsParams *params)
{
    Mat region, image, fourChannelImage, writeRegion;

    Rect rect = params->roi;

    image = boost::any_cast<Mat>(params->param1);

    cvtColor( image, fourChannelImage, COLOR_BGR2BGRA );

    region = params->src(Rect(rect.x, rect.y, rect.width, rect.height));

    for (int i = 0; i<rect.width; i++)
    {
        for (int j = 0; j<rect.height; j++)
        {
            Vec4b &pixel = fourChannelImage.at<Vec4b>(j,i);
            if (pixel[0] >= 240 && pixel[1] >= 240 && pixel[2] >= 240)
            {
               // pixel[0] = 0;
               // pixel[1] = 0;
               // pixel[2] = 0;
                pixel[3] = 255;
            }

            //region.at<Vec4b>(j,i) = region.at<Vec4b>(j,i) + fourChannelImage.at<Vec4b>(j,i);
        }
    }

    double alpha = 0.5f;
    double beta = (1 - alpha);

    addWeighted(fourChannelImage, alpha, region, beta, 0.0, writeRegion);

    writeRegion.copyTo(region);

}


Mat MatrixOperations::KernelMat(float data[], int dimX, int dimY)
{
    return Mat(dimX, dimY, CV_32F, data);
}

Mat MatrixOperations::KernelMatBoxFilter(int dSize)
{
    return Mat::ones(dSize, dSize, CV_32F)/(float)(dSize*dSize);
}


