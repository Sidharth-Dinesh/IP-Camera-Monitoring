/**
* @file 
* @brief 
*  
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include <errno.h>
#include <cstdio>

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
#include "si/video.h"

using namespace std;
using namespace cv;

void object_detection(Mat inputImage)
{
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
    std::string b64Img(drawObjectsJson["DetectedObjectsJSON"]["Image"].GetString(),drawObjectsJson["DetectedObjectsJSON"]["Image"].GetStringLength());
    cv::Mat image = base642Mat(b64Img);
    cv::imwrite("objectDetectOut.png", image);
}

void object_detection(Mat inputImage)
{
    if (inputImage.empty()) {
        std::cerr << "Error! Unable to read input image" << std::endl;
    }

    /* ---- Create Input JSON ---- */
    std::string b64InpImg = mat2Base64(inputImage);
    rapidjson::Document inputJson;
    inputJson.SetObject();
    rapidjson::Value nprob;
    rapidjson::Value inpImage;

    /* Set top N Class predictions, N=1 (below) */
    nprob.SetString("1");
    inpImage.SetString(b64InpImg.c_str(), strlen(b64InpImg.c_str()), inputJson.GetAllocator());
    inputJson.AddMember("inputImage", inpImage, inputJson.GetAllocator());
    inputJson.AddMember("nprob", nprob, inputJson.GetAllocator());

    /*################ Call classifyObjects Component #####################*/
    subprocess::popen classifyObjects("/usr/bin/classifyObjects", {});
    classifyObjects.stdin() << jsonDoc2Text(inputJson) << std::endl;
    classifyObjects.close();
    std::string classifyObjectsOut;
    classifyObjects.stdout() >> classifyObjectsOut;

    /*#####################################################################*/
       /* ---- Print the output  ---- */
    rapidjson::Document classifyObjectsJson = readJsonString(classifyObjectsOut);
    double score = classifyObjectsJson["ClassifiedObjsJSON"][0]["Class0"]["Score"].GetDouble();
    int index = classifyObjectsJson["ClassifiedObjsJSON"][0]["Class0"]["Index"].GetInt();
    std::cout<<"Top 1 prediction: "<< "\nClass Score: " << score <<"\nClass Index: " << index<<"\n";
}

int main() {

    /* Initialize the shunya interfaces library */
    initLib();
    
    captureObj src;
    src = newCaptureDevice("camera"); /* Create capture Instance */

    cout << "OpenCV version : " << CV_VERSION << endl;
    cout << "Major version : " << CV_MAJOR_VERSION << endl;
    cout << "Minor version : " << CV_MINOR_VERSION << endl;
    cout << "Subminor version : " << CV_SUBMINOR_VERSION << endl;

    Mat frame; /* Variable to store frame */

    readCaptureSettings(&src);

    while(1) {

        frame  = captureFrameToMem(&src); /* Capture one frame at a time in a loop*/

        if (frame.empty()){
            fprintf(stderr, "End of video file!.");
            closeCapture(&src);
            return 0;
        }

        auto start = chrono::high_resolution_clock::now();  /* start timer */

        object_detection(frame);
        object_classification(frame);

        auto stop = chrono::high_resolution_clock::now();  /* stop timer */

        /* calculate the time difference in microseconds */
        auto duration = chrono::duration_cast<chrono::microseconds> (stop - start);

        /* print duration at every 100th iteration starting from 1st iteration */
        if (i == 0) {
            cout << "Time for detection: " << duration.count() << " microseconds\n";
        }

        i = (i+1) % 100; /* update i */

        /* ##################################################################### */
        
        /* ---- Send data to influxDB ---- */
        unsigned long currTime = time(NULL); /* Get current timestamp */

        /* Create an InfluxDB Object */
        influxdbObj testdb = newInfluxdb("test-db");
    
        while(1)
        {
            /* Store data into InfluxDB database */
            writeDbInflux (testdb, "equipment, equipmentFailure=%d,temperature=%f,id=%s %ld", value1, value2, value3, currTime);

            //Please change these above values according to outputs of Object Detection.
        }

        imshow("Image",frame);
        if (waitKey(10) == 27) {
            break;
        }

        delay(10000); /* Wait for 10 secs before storing another value */
    }
    
    closeCapture(&src);
    return 0;
}

