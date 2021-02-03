/**
 * @file exutils.cpp
 * @brief Utility functions for Shunya AI
 */

#include "exutils.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <math.h>
#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>


/* --- RapidJson Includes --- */
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/error/en.h"

#include "subprocess.hpp"

using namespace std;


/**
 * @brief Run the shell command and return the result.
 *
 * @param cmd Shell command
 * @return std::string stdout string.
 */
std::string runCommand(std::string cmd)
{
    std::string data;
    FILE *stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");
    stream = popen(cmd.c_str(), "r");

    if (stream) {
        while (!feof(stream)){
            if (fgets(buffer, max_buffer, stream) != NULL) {
                data.append(buffer);      
            } 

        }

        pclose(stream);
    }

    return data;
}

/**
 * @brief Read json file
 *
 * @param filename JSON file name
 * @return rapidjson::Document JSON document
 */
rapidjson::Document readJsonFile(std::string filename)
{
    /* Open the file in read mode */
    FILE *fp = fopen(filename.c_str(), "rb");
    /* Declare read buffer */
    char readBuffer[65536];
    /* Declare stream for reading the example stream */
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    /* Declare a JSON document. JSON document parsed will be stored in this variable */
    rapidjson::Document doc;
    /* Parse json file and store it in `doc` */
    rapidjson::ParseResult ok = doc.ParseStream(is);

    /* Check if the parsing caused errors*/
    if (!ok) {
        fprintf(stderr, "Error Reading JSON file: JSON parse error: %s (%lu)\n",
                GetParseError_En(ok.Code()), ok.Offset());
    }

    /* Close the example.json file*/
    fclose(fp);
    return doc;
}

/**
 * @brief Read Json string
 *
 * @param json json string
 * @return rapidjson::Document JSON document
 */
rapidjson::Document readJsonString(std::string json)
{
    rapidjson::Document doc;
    /* Parse json file and store it in `doc` */
    rapidjson::ParseResult ok = doc.Parse(json.c_str());

    /* Check if the parsing caused errors*/
    if (!ok) {
        fprintf(stderr, "Error Reading JSON string: JSON parse error: %s (%lu)\n",
                GetParseError_En(ok.Code()), ok.Offset());
    }

    return doc;
}

/**
 * @brief Decode the base64 string
 *
 * @param Data Data to be decoded
 * @param DataByte Length of the decoded data
 * @return std::string Image in string array
 */
static std::string base64Decode(const char *Data, int DataByte)
{
    /* Decoding table */
    const char DecodeTable[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        62, // '+'
        0, 0, 0,
        63, // '/'
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
        0, 0, 0, 0, 0, 0,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
    };
    std::string strDecode;
    int nValue;
    int i = 0;

    while (i < DataByte) {
        if (*Data != '\r' && *Data != '\n') {
            nValue = DecodeTable[*Data++] << 18;
            nValue += DecodeTable[*Data++] << 12;
            strDecode += (nValue & 0x00FF0000) >> 16;

            if (*Data != '=') {
                nValue += DecodeTable[*Data++] << 6;
                strDecode += (nValue & 0x0000FF00) >> 8;

                if (*Data != '=') {
                    nValue += DecodeTable[*Data++];
                    strDecode += nValue & 0x000000FF;
                }
            }

            i += 4;

        } else { // carriage return, skip
            Data++;
            i++;
        }
    }

    return strDecode;
}

/**
 * @brief Convert base64 string to OpenCV Mat format
 *
 * @param base64_data Base64 string
 * @return cv::Mat OpenCV Mat image
 */
cv::Mat base642Mat(std::string &base64_data)
{
    cv::Mat img;
    std::string s_mat;
    s_mat = base64Decode(base64_data.data(), base64_data.size());
    std::vector<char> base64_img(s_mat.begin(), s_mat.end());
    img = cv::imdecode(base64_img,cv::IMREAD_COLOR);
    return img;
}


static std::string base64Encode(const unsigned char *Data, int DataByte)
{
    // code table
    const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    //return value
    std::string strEncode;
    unsigned char Tmp[4] = { 0 };
    int LineLength = 0;

    for (int i = 0; i < (int)(DataByte / 3); i++) {
        Tmp[1] = *Data++;
        Tmp[2] = *Data++;
        Tmp[3] = *Data++;
        strEncode += EncodeTable[Tmp[1] >> 2];
        strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
        strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
        strEncode += EncodeTable[Tmp[3] & 0x3F];

        if (LineLength += 4, LineLength == 76) {
            strEncode += "\r\n";
            LineLength = 0;
        }
    }

    // Encode the remaining data
    int Mod = DataByte % 3;

    if (Mod == 1) {
        Tmp[1] = *Data++;
        strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
        strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
        strEncode += "==";

    } else if (Mod == 2) {
        Tmp[1] = *Data++;
        Tmp[2] = *Data++;
        strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
        strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
        strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
        strEncode += "=";
    }

    return strEncode;
}

std::string mat2Base64(const cv::Mat &img)
{
    std::string img_data;
    std::vector<uchar> vecImg;
    std::vector<int> vecCompression_params;
    vecCompression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    vecCompression_params.push_back(90);
    cv::imencode(".jpeg",img, vecImg, vecCompression_params);
    img_data = base64Encode(vecImg.data(), vecImg.size());
    return img_data;
}

/**
 * @brief JSON document to JSON string
 *
 * @param doc JSON doc
 * @return const char* JSON string
 */
const char *jsonDoc2Text(rapidjson::Document& doc)
{
    rapidjson::StringBuffer buffer;
    buffer.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(18);
    doc.Accept(writer);
    return strdup( buffer.GetString() );
}
