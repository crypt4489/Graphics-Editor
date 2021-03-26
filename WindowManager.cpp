#include "WindowManager.h"


WindowManager::WindowManager(String name)
{
    this->window_name = name;
    this->leftClicked = false;

    this->mo = new MatrixOperations();
    this->mo->createLookupTable(10, this->mo->table);

}


WindowManager::~WindowManager()
{
    delete this->mo;
}


void WindowManager::createWindow()
{
    namedWindow(this->window_name, WINDOW_AUTOSIZE);

    setMouseCallback(this->window_name, mouseCallBackFunc, this);
}

void WindowManager::displayWindow()
{
    imshow(this->window_name, this->frame);
}

int WindowManager::wait(int milliseconds)
{
    if (milliseconds == 0) {
        return waitKey();
    }

    return waitKey(milliseconds);
}

void WindowManager::initializeRect(double width, double height, double x, double y)
{
    Rect rect = Rect(x, y, width, height);

    this->riv.push_back(rect);

    this->rs.push_back(0);
}

int WindowManager::checkPointerAgainstRect(double x, double y, Rect rect)
{
    double xRect, yRect, width, height;

    xRect = rect.x;
    yRect = rect.y;
    width = rect.width;
    height = rect.height;

    double rectFarX = xRect+width;
    double rectFarY = yRect+height;

    double sensitivity = 15.0;

    if ((x > xRect+sensitivity) && (x < rectFarX-sensitivity) && (y > yRect+sensitivity) && (y < rectFarY-sensitivity))
    {
        return POINTER_ON_CENTER;
    }
    else if  ((x > xRect) && (x < (xRect + sensitivity)) && (y > yRect) && (y < (rectFarY-(height/2.0))))
    {
        return POINTER_ON_LEFT_UPPER_CORNER;
    }
    else if ((x > xRect) && (x < (xRect + sensitivity)) && (y < rectFarY) && (y > (rectFarY-(height/2.0))))
    {
        return POINTER_ON_LEFT_LOWER_CORNER;
    }
    else if  ((x < rectFarX) && (x > (rectFarX - sensitivity)) && (y > yRect) && (y < (rectFarY-(height/2.0))))
    {
        return POINTER_ON_RIGHT_UPPER_CORNER;
    }
    else if ((x < rectFarX) && (x > (rectFarX - sensitivity)) && (y < rectFarY) && (y > (rectFarY-(height/2.0))))
    {
        return POINTER_ON_RIGHT_LOWER_CORNER;
    }

    return 0;
}

Mat WindowManager::loadImage(string imgPath)
{
    Mat img = imread(imgPath, IMREAD_COLOR);
    return img;
}

void WindowManager::moveROI(double x, double y, Rect& rect)
{
    double diffX = x - this->mouseX;

    double diffY = y - this->mouseY;

    rect.x = rect.x + diffX;
    rect.y = rect.y + diffY;
}

void WindowManager::resizeROI(double x, double y, Rect& rect, int state)
{
    switch(state)
    {
    case POINTER_ON_LEFT_UPPER_CORNER:
        rect.x = rect.x+(x-this->mouseX);
        rect.width = rect.width-(x-this->mouseX);
        rect.height = rect.height-(y-this->mouseY);
        rect.y = rect.y+(y-this->mouseY);

        break;
    case POINTER_ON_LEFT_LOWER_CORNER:
        rect.x = rect.x+(x-this->mouseX);
        rect.width = rect.width-(x-this->mouseX);
        rect.height = rect.height+(y-this->mouseY);

        break;
    case POINTER_ON_RIGHT_LOWER_CORNER:
        rect.width = rect.width+(x-this->mouseX);
        rect.height = rect.height+(y-this->mouseY);

        break;
    case POINTER_ON_RIGHT_UPPER_CORNER:
        rect.width = rect.width+(x-this->mouseX);
        rect.height = rect.height-(y-this->mouseY);
        rect.y = rect.y+(y-this->mouseY);
        break;
    default:
        break;
    }
}

void WindowManager::addImageFunc(string cmdline)
{
    vector<string> arguments;

    boost::split(arguments, cmdline, [](char c){return c ==' ';});

    if (arguments.size() < 2)
        return;

    if ("save" == arguments[0])
    {
        WindowManager::windowFunctionPointer func = &WindowManager::saveImage;
        WindowFunction windowFunc = { func, { arguments[1], NULL} };
        this->wfs.push_back(windowFunc);
        this->performWindowFunction = true;
        return;
    }

    if ("superimpose" == arguments[0])
    {
        Mat newImage = loadImage(arguments[1]);
        double width = newImage.cols;
        double height = newImage.rows;
        this->initializeRect(width, height, 100, 100);

        MatrixOperations::matrixFunctionPointer func = &MatrixOperations::SuperimposeImage;

        FrameFunction frameFunc = { func, {this->frame, this->riv[this->riv.size()-1], newImage, 0.0} };

        this->functionsParamsMap.push_back(pair<int, FrameFunction>(this->riv.size()-1, frameFunc));

        this->performFrameFunction = true;

        return;
    }

    int roi_pos = stoi(arguments[1], nullptr);

    if ("boxFilter" == arguments[0])
    {
        int kernelSize = stoi(arguments[2], nullptr);
        MatrixOperations::matrixFunctionPointer func = &MatrixOperations::ApplyFilterToROI;
        FrameFunction frameFunc = { func, {this->frame, this->riv[roi_pos-1], this->mo->KernelMatBoxFilter(kernelSize), NULL} };
        this->functionsParamsMap.push_back(pair<int , FrameFunction>(roi_pos-1, frameFunc));
    }
    else if ("scan" == arguments[0])
    {
        MatrixOperations::matrixFunctionPointer func = &MatrixOperations::ScanImageROI;
        FrameFunction frameFunc = { func, {this->frame, this->riv[roi_pos-1],  this->mo->table, NULL} };
        this->functionsParamsMap.push_back(pair<int, FrameFunction>(roi_pos-1, frameFunc));
    }
    else if ("brighten" == arguments[0])
    {
        double alpha = stod(arguments[2], nullptr);
        double beta = stod(arguments[3], nullptr);
        MatrixOperations::matrixFunctionPointer func = &MatrixOperations::ApplyBrightenToROI;
        FrameFunction frameFunc = { func, {this->frame, this->riv[roi_pos-1], alpha, beta} };
        this->functionsParamsMap.push_back(pair<int , FrameFunction>(roi_pos-1, frameFunc));
    }
    else if ("grayscale" == arguments[0])
    {
        MatrixOperations::matrixFunctionPointer func = &MatrixOperations::GrayScaleROI;
        FrameFunction frameFunc = { func, {this->frame, this->riv[roi_pos-1], NULL, NULL} };
        this->functionsParamsMap.push_back(pair<int, FrameFunction>(roi_pos-1, frameFunc));
    }
    else if ("reverse" == arguments[0])
    {
        MatrixOperations::matrixFunctionPointer func = &MatrixOperations::ReverseROI;
        FrameFunction frameFunc = { func, {this->frame, this->riv[roi_pos-1], NULL, NULL} };
        this->functionsParamsMap.push_back(pair<int, FrameFunction>(roi_pos-1, frameFunc));
    }
    else if ("mirror_left" == arguments[0])
    {
        MatrixOperations::matrixFunctionPointer func = &MatrixOperations::MirrorLeftHalf;
        FrameFunction frameFunc = { func, {this->frame, this->riv[roi_pos-1], NULL, NULL} };
        this->functionsParamsMap.push_back(pair<int, FrameFunction>(roi_pos-1, frameFunc));
    }
    else if ("zoom" == arguments[0])
    {
        MatrixOperations::matrixFunctionPointer func = &MatrixOperations::ZoomROI;
        FrameFunction frameFunc = { func, {this->frame, this->riv[roi_pos-1], NULL, NULL} };
        this->functionsParamsMap.push_back(pair<int, FrameFunction>(roi_pos-1, frameFunc));
    }
    else if ("pixelate" == arguments[0])
    {
        MatrixOperations::matrixFunctionPointer func = &MatrixOperations::PixelateROI;

        struct timeval frame_init;

        gettimeofday(&frame_init, NULL);

        FrameFunction frameFunc = { func, {this->frame, this->riv[roi_pos-1], frame_init, 0.0} };
        this->functionsParamsMap.push_back(pair<int, FrameFunction>(roi_pos-1, frameFunc));
    }
    this->performFrameFunction = true;
}

void WindowManager::setWindowCoords(double x, double y)
{
    this->mouseX = x;
    this->mouseY = y;
}

void WindowManager::setFrame(Mat frame)
{
    this->frame = frame.clone();
}

void WindowManager::setBaseImage(Mat baseImage)
{
    Mat fourChannelBase;
    cvtColor( baseImage, fourChannelBase, COLOR_BGR2BGRA );
    this->baseImage = fourChannelBase.clone();
    this->setFrame(this->baseImage);
}


void WindowManager::processFrame()
{
    this->performFrameFunction = false;

    this->setFrame(this->baseImage);

    for (const auto &func : this->functionsParamsMap)
    {
        FrameFunction func_l = func.second;
        if(func.first != -1)
        {
            func_l.params.roi = this->riv[func.first];
        }
        (this->mo->*(func_l.func))(&(func_l.params));
    }

    this->drawRois();
}

void WindowManager::postProcessFrame()
{
    this->performWindowFunction = false;

    while(!this->wfs.empty())
    {
        auto func_w = this->wfs.back();
        (this->*(func_w.func))(&(func_w.params));
        this->wfs.pop_back();
    }
}

void WindowManager::handleLeftClick(double x, double y)
{
    this->leftClicked = true;
    int iter = 0;
    for (Rect &rect: this->riv)
    {
        int state = this->checkPointerAgainstRect(x, y, rect);
        this->rs[iter] = state;
        iter++;
    }
}

void WindowManager::releaseLeftClick()
{
    this->leftClicked = false;
}

void WindowManager::saveImage(WindowManager::WindowManagerParams *params)
{
    string saveFile = boost::any_cast<string>(params->param1);
    imwrite(saveFile, this->frame);
}

void WindowManager::updateRois(double x, double y)
{
    if (this->leftClicked)
    {
        bool found = false;
        int iter = 0;
        for (Rect &rect: this->riv)
        {
            int state = this->rs[iter];
            iter++;
            if (state == 0)
            {
                continue;
            }
            if (state == POINTER_ON_CENTER)
            {
                this->moveROI(x, y, rect);
                found = true;
            }
            else
            {
                this->resizeROI(x, y, rect, state);
                found = true;
            }
            if(found)
                break;
        }
    }
}

void WindowManager::drawRois()
{
    if (this->addRectangle)
    {
        this->addRectangle = false;
    }

    Mat &drawnFrame = this->frame;
    for (const auto& rect : this->riv)
    {
        rectangle(drawnFrame, rect, Scalar( 0, 0, 255 ), 1, LINE_8);
    }
    this->frame = drawnFrame;
}

bool WindowManager::getLeftClicked()
{
    return this->leftClicked;
}

bool WindowManager::getPerformFrameFunction()
{
    return this->performFrameFunction;
}

bool WindowManager::getPerformWindowFunction()
{
    return this->performWindowFunction;
}

bool WindowManager::getAddRectangle()
{
    return this->addRectangle;
}

void WindowManager::setAddRectangle(bool state)
{
    this->addRectangle = state;
}

void mouseCallBackFunc(int event, int x, int y, int flags, void* userdata)
{
     WindowManager *inst = reinterpret_cast<WindowManager*>(userdata);

     double xD = static_cast<double>(x);
     double yD = static_cast<double>(y);

     if  ( event == EVENT_LBUTTONDOWN )
     {
        inst->handleLeftClick(xD, yD);
     }
     else if  ( event == EVENT_RBUTTONDOWN )
     {
        inst->initializeRect(100, 200, xD, yD);
        inst->setAddRectangle(true);
     }
     else if  ( event == EVENT_MBUTTONDOWN )
     {

     }
     else if ( event == EVENT_MOUSEMOVE )
     {

     }
     else if ( event == EVENT_LBUTTONUP)
     {
        inst->releaseLeftClick();
     }

     if (inst->getLeftClicked() || inst->getPerformFrameFunction() || inst->getAddRectangle())
     {
        inst->updateRois(xD, yD);
        inst->processFrame();
     }

     if (inst->getPerformWindowFunction())
     {
        inst->postProcessFrame();
     }

     inst->displayWindow();

     inst->setWindowCoords(xD, yD);
}
