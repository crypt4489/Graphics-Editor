#include "WindowManager.h"
#include "MatrixOperations.h"
#include <pthread.h>
#include <cstdlib>
#include <string>


using namespace cv;
using namespace std;


pthread_mutex_t mainMutex;
string global_input;

void *receiveInput(void *arg)
{
    bool loop = true;
    string input;
    while(loop) {
        getline(cin, input);
        if (input == "exit") {
            loop = false;
            break;
        }
        pthread_mutex_lock(&mainMutex);
        global_input = input;
        pthread_mutex_unlock(&mainMutex);
    }

    pthread_exit((void*)0);

}

int main(int argc, char *argv[])
{

    CommandLineParser parser (argc, argv,
    "{@input | sky.png | input image}");


    float data[9] = { 0, -1, 0, -1, 5, -1, 0, -1, 0};
    Mat frame;
    pthread_t stdInputThread;
    string window_name, input;

    window_name = "Feed";
    input = "";

    int pRet = pthread_create(&stdInputThread, NULL, receiveInput, nullptr);

    if (pRet) {
        throw runtime_error("Cannot create thread");
        return EXIT_FAILURE;
    }

    pthread_mutex_init(&mainMutex, NULL);

    WindowManager *wm = new WindowManager(window_name);

	wm->createWindow();

    frame = imread(samples::findFile(parser.get<String>("@input")), IMREAD_COLOR);

    wm->setBaseImage(frame);

    while(true) {

        if (input != "") {
            wm->addImageFunc(input);
            input = "";
        }


        pthread_mutex_lock(&mainMutex);
        input = global_input;
        global_input = "";
        pthread_mutex_unlock(&mainMutex);


        if (wm->wait(20) == 27)
        {
            cout << "exiting video feed" << endl;
            break;
        }
    }

    delete wm;

    pthread_mutex_destroy(&mainMutex);

    pthread_exit(NULL);

    return EXIT_SUCCESS;
}

