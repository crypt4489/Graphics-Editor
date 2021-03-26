#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#define POINTER_ON_CENTER 1
#define POINTER_ON_LEFT_LOWER_CORNER 2
#define POINTER_ON_LEFT_UPPER_CORNER 3
#define POINTER_ON_RIGHT_LOWER_CORNER 4
#define POINTER_ON_RIGHT_UPPER_CORNER 5


#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/any.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

#include "MatrixOperations.h"


using namespace std;
using namespace cv;



void mouseCallBackFunc(int event, int x, int y, int flags, void* userData);

class WindowManager
{
public:

    WindowManager(String name);
    virtual ~WindowManager();

    void createWindow();
    void displayWindow();
    int wait(int milliseconds);

    void initializeRect(double width, double height, double x, double y);
    void addImageFunc(string cmdline);
    void setWindowCoords(double x, double y);
    void setFrame(Mat frame);
    void setBaseImage(Mat baseImage);
    Mat loadImage(string imgPath);
    void processFrame();
    void postProcessFrame();
    void handleLeftClick(double x, double y);
    void releaseLeftClick();
    void updateRois(double x, double y);
    void setAddRectangle(bool state);
    bool getPerformFrameFunction();
    bool getLeftClicked();
    bool getPerformWindowFunction();
    bool getAddRectangle();


protected:

private:

    struct FrameFunction
    {
        MatrixOperations::matrixFunctionPointer func;
        MatrixOperations::MatrixOperationsParams params;
    };


    struct WindowManagerParams
    {
        boost::any param1;
        boost::any param2;
    };

    typedef void (WindowManager::*windowFunctionPointer)(WindowManagerParams*);

    struct WindowFunction
    {
        WindowManager::windowFunctionPointer func;
        WindowManager::WindowManagerParams params;
    };

    typedef vector<pair<int, FrameFunction>> imageFunctionMap;
    typedef vector<WindowFunction> windowFunctionStack;
    typedef vector<Rect> roiInView;
    typedef vector<int> roiStates;



    void moveROI(double x, double y, Rect& rect);
    void resizeROI(double x, double y, Rect& rect, int state);
    int checkPointerAgainstRect(double x, double y, Rect rect);
    void drawRois();
    void clearRoiState();
    void saveImage(WindowManagerParams *params);

    Mat frame, baseImage;
    MatrixOperations *mo;
    bool leftClicked, performFrameFunction, performWindowFunction, addRectangle;
    String window_name;
    double mouseX, mouseY;
    imageFunctionMap functionsParamsMap;
    windowFunctionStack wfs;
    roiInView riv;
    roiStates rs;
};

#endif // WINDOWMANAGER_H
