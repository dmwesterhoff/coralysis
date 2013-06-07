/*  filename:   imgutil.h
 *  author:     David M. Westerhoff
 *  version:    alpha
 *  last mod:   3/18/13
 *  descript:   header file for imgutil class that does the behind the scene
 *              image rendering
 */

#ifndef _IMGUTIL_H
#define _IMGUTIL_H

#include "formatter.h"
// opencv headers
#include <cv.h>
#include <highgui.h>
#include <imgproc/imgproc.hpp>
// c++ headers
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
// boost headers
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

class imgutil {
private:

// class formats data for output
    friend class formatter;

//  runtime flags for all available methods
    bool doColors, doSumLaplace, doSumCanny, doSumBinLaplace,
    doSumFourier, doSumBinFourier, doSumLonersFourier, doSumBinLonersFourier;

// structure cvcontainer holds data for the image analysis
    struct cvcontainer {
        cv::Mat data;   // contains the image matrix
        cv::Mat data_8UC3;  // CV_8U3C type image matrix (8bit 3 channels)
        int height,width,depth,dimension,channels,type;
        std::vector<cv::Mat> bgr_planes; // vector of all image channels
        cv::Mat gray_channel, blue_channel, green_channel, red_channel;
        cv::Mat gray_channel_32F, blue_channel_32F, green_channel_32F, red_channel_32F;
        cv::Mat fourier_gray, fourier_blue, fourier_green, fourier_red;
        cv::Mat laplace_all, laplace_blue, laplace_green, laplace_red;
        cv::Scalar mean_image;
        double mean_blue, mean_green, mean_red;
        int median_blue, median_green, median_red;
        double sumLaplace_all, sumLaplace_blue, sumLaplace_green, sumLaplace_red;
        std::vector<double> sumCanny_all, sumCanny_blue, sumCanny_green, sumCanny_red;
        std::vector<double> sumBinLaplace_blue, sumBinLaplace_green, sumBinLaplace_red;
    };

    std::string name;        // name given is derived from filename

//  image matrices required for image processing
    cv::Mat image;  // raw image matrix
    cv::Mat image_norm; // norm image matrix
	cvcontainer base;   // starting base image
	cvcontainer norm;   // starting normalize image [0-255]

    void init_channels(cvcontainer &);   // initialize channels to size and type
	void split_channels(cvcontainer &);  // splits image into channels
	void fourier_transform(cvcontainer &);
	void analyze_colors(cvcontainer &);  // calculates median and mean
	void sumLaplace(cvcontainer &);
	void sumCanny(cvcontainer &);
	void sumBinLaplace(cvcontainer &);
	void sumBinLonersFourier(cvcontainer &);

    // utility methods for class
    void get_info(cvcontainer &);
    void get_info(cv::Mat &);
    void show_image(cv::Mat &);          // prints image to screen
    void is_workable(cvcontainer &);     // sanity check images
	void get_medians(cvcontainer &);     // finds medians for all channels
	void normalize(cvcontainer &in);
	std::string get_depth(int);          // returns image type e.g. CV_8U

public:

	imgutil(std::string);	// ctor takes filename as only argument

};
#endif
