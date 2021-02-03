/**
 * @file object_detect.cpp
 * @brief Example for object detection using Shunya AI library binaries
 */

#include <iostream>

#include <cstdio>
#include <cstdlib>


#include <string>
#include <errno.h>

#include "si/video.h"

/* --- RapidJson Includes --- */
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/filereadstream.h"

#include "subprocess.hpp"
#include <opencv2/opencv.hpp>

#include "exutils.h"

using namespace std;
using namespace cv;

void detect(Mat frame)
{
    /* --- Take Input image --- */
    cv::Mat inputImage = frame;

    if (inputImage.empty()) {
        std::cerr << "Error! Unable to read input image" << std::endl;
    }

    /* ---- Create Input JSON ---- */
    std::string b64InpImg = mat2Base64(inputImage);
    rapidjson::Document inputJson;
    inputJson.SetObject();
    rapidjson::Value probability;
    rapidjson::Value inpImage;
    probability.SetString("0.8");
    inpImage.SetString(b64InpImg.c_str(), strlen(b64InpImg.c_str()), inputJson.GetAllocator());
    inputJson.AddMember("inputImage", inpImage, inputJson.GetAllocator());
    inputJson.AddMember("probability", probability, inputJson.GetAllocator());
    /*############### Call detectObjects Component ##################*/
    subprocess::popen detectObjects("/usr/bin/detectObjects", {});
    detectObjects.stdin() << jsonDoc2Text(inputJson) << std::endl;
    detectObjects.close();
    std::string detectObjectsOut;
    detectObjects.stdout() >> detectObjectsOut;
    /*###############################################################*/

    /*############### Call drawObjects Component ##################*/
    subprocess::popen drawObjects("/usr/bin/drawObjects", {});
    drawObjects.stdin() << detectObjectsOut << std::endl;
    drawObjects.close();
    std::string drawObjectsOut;
    drawObjects.stdout() >> drawObjectsOut;
    /*###############################################################*/
    /* ---- Parse draw object output  ---- */
    rapidjson::Document drawObjectsJson = readJsonString(drawObjectsOut);
    std::string b64Img(drawObjectsJson["DetectedObjectsJSON"]["Image"].GetString(),
                       drawObjectsJson["DetectedObjectsJSON"]["Image"].GetStringLength());
    cv::Mat image = base642Mat(b64Img);
}

int main() {

    captureObj src;

    src = newCaptureDevice("camera"); /* Create capture Instance */

    storeObj videoStore;  /* Video Store instance */

    videoStore = newVideoStore("videoStore"); /* Create new Video store */

    cout << "OpenCV version : " << CV_VERSION << endl;
    cout << "Major version : " << CV_MAJOR_VERSION << endl;
    cout << "Minor version : " << CV_MINOR_VERSION << endl;
    cout << "Subminor version : " << CV_SUBMINOR_VERSION << endl;

    Mat frame; /* Variable to store frame */

    readCaptureSettings(&src);

    while(1) {

        frame  = captureFrameToMem(&src); /* Capture the frame from source */

        if (frame.empty()){
            fprintf(stderr, "End of video file!.");
            closeCapture(&src);
            return 0;
        }

        detect(frame);

        storeStreamToDisk(&videoStore, frame, "file.avi");  /* Store frame into file.avi*/
    }

    closeCapture(&src);
    closeStore(&videoStore);
    
    return 0;
}
