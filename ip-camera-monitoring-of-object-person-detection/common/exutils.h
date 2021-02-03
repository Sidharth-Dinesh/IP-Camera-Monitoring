/**
 * @file   	exutils.h
 * @brief  	Utility functions for Shunya AI examples
 */
/*! Include Guard */
#ifndef __EXUTILS__H__
#define __EXUTILS__H__

#include <iostream>
#include <cstdlib>

#include <string>
#include <opencv2/opencv.hpp>

/* --- RapidJson Includes --- */
#include "rapidjson/document.h"

/**
 * @brief Run the shell command and return the result.
 * 
 * @param cmd Shell command 
 * @return std::string stdout string.
 */
extern std::string runCommand(std::string cmd);

/**
 * @brief Convert base64 string to OpenCV Mat format
 *
 * @param base64_data Base64 string
 * @return cv::Mat OpenCV Mat image
 */
extern cv::Mat base642Mat(std::string &base64_data);

/**
 * @brief Read Json string 
 * 
 * @param json json string 
 * @return rapidjson::Document JSON document
 */
extern rapidjson::Document readJsonString(std::string json);

/**
 * @brief Read json file
 * 
 * @param filename JSON file name
 * @return rapidjson::Document JSON document 
 */
extern rapidjson::Document readJsonFile(std::string filename);

/**
 * @brief JSON document to JSON string
 *
 * @param doc JSON doc
 * @return const char* JSON string
 */
const char *jsonDoc2Text(rapidjson::Document& doc);

extern std::string mat2Base64(const cv::Mat &img);

std::string runCommandPstr(std::string cmd, const char* stdInp);

#endif  /* __EXUTILS__H__ */ /* End of Include Guard */
