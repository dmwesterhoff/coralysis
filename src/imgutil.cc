/*  filename:   imgutil.cc
 *  author:     David M. Westerhoff
 *  version:    alpha
 *  last mod:   3/18/13
 *  descript:   implementation of the imgutil class, the class is
 *              uses mostly OpenCV to parse the input image.
 */

#include "imgutil.h"

#define MCC CV_MAKETYPE(image.depth(),3)    // 3 channel M.T. same depth
#define SCC CV_MAKETYPE(image.depth(),1)    // single channel M.T. same depth
#define SC32F CV_MAKETYPE(CV_32F,1)         // single channel with 32F depth
#define HIST_SIZE 256
#define BLUE_LAYER 0
#define GREEN_LAYER 1
#define RED_LAYER 2

using namespace cv;
using namespace std;


// public methods
imgutil::imgutil(string filename) {

    // initialize flags
    doColors = true;
    doSumLaplace = true;
    doSumCanny = true;
    doSumBinLaplace = true;
    doSumFourier = false;           // not implemented
    doSumBinFourier = false;        // not implemented
    doSumLonersFourier = false;     // not implemented
    doSumBinLonersFourier = false;  // not implemented

    // initialize base and get image data
    name = filename;
	image = imread(filename,1);
	image_norm = imread(filename,1);
	base.data = image;
	norm.data = image_norm;
	// setting up containers for base and norm
	base.height = image.rows;
	base.width = image.cols;
    base.depth = image.depth();
	base.dimension = image.dims;
    base.channels = image.channels();
	base.type = image.type();
    norm.height = image.rows;
    norm.width = image.cols;
    norm.depth = image.depth();
    norm.dimension = image.dims;
    norm.channels = image.channels();
    norm.type = image.type();

	// CV's NORMALIZE
    // cv::normalize(base.data, norm.data, 0, 255, NORM_MINMAX);
    // NORMALIZE
	normalize(norm);
	show_image(base.data);
	show_image(norm.data);

    is_workable(base);  // check to see if valid image
    is_workable(norm);

    // get image info and print to term
    //get_info(base);
    //get_info(norm);

    // initialize channels
    init_channels(base);
    init_channels(norm);

	// split channels gray, blue, green, red
	split_channels(base);
	split_channels(norm);

	// forward dft transform on all channels
	fourier_transform(base);
	fourier_transform(norm);

	if (doColors) {
	    analyze_colors(base);
	    analyze_colors(norm);
	}
	if (doSumLaplace) {
	    sumLaplace(base);
	    sumLaplace(norm);
	}
	if (doSumCanny) {
	    sumCanny(base);
	    sumCanny(norm);
	}
	if (doSumBinLaplace) {
	    sumBinLaplace(base);
	    sumBinLaplace(norm);
	}
}


// creates matrices of correct size and type
void imgutil::init_channels(cvcontainer &c) {

    c.data.convertTo(c.data_8UC3, CV_8UC3); // get 8UC3 typed matrix

    c.gray_channel.create(c.height, c.width, SCC);
    c.blue_channel.create(c.height, c.width, SCC);
    c.green_channel.create(c.height, c.width, SCC);
    c.red_channel.create(c.height, c.width, SCC);

    c.fourier_gray.create(c.height, c.width, SC32F);
    c.fourier_blue.create(c.height, c.width, SC32F);
    c.fourier_green.create(c.height, c.width, SC32F);
    c.fourier_red.create(c.height, c.width, SC32F);

    c.laplace_all.create(c.height, c.width, c.type);
    c.laplace_blue.create(c.height, c.width, SCC);
    c.laplace_green.create(c.height, c.width, SCC);
    c.laplace_red.create(c.height, c.width, SCC);

}

// splits image into all channels, and gets 32F typed version as well
void imgutil::split_channels(cvcontainer &c) {

      split(c.data, c.bgr_planes);
      cvtColor(c.data, c.gray_channel, CV_BGR2GRAY);
      c.blue_channel = c.bgr_planes[BLUE_LAYER];
      c.green_channel = c.bgr_planes[GREEN_LAYER];
      c.red_channel = c.bgr_planes[RED_LAYER];
      c.blue_channel.convertTo(c.blue_channel_32F, CV_32FC1);
      c.green_channel.convertTo(c.green_channel_32F, CV_32FC1);
      c.red_channel.convertTo(c.red_channel_32F, CV_32FC1);
      c.gray_channel.convertTo(c.gray_channel_32F, CV_32FC1);
      // now have all channels, and 32FC1 typed matrix of each channel.

      Laplacian(c.data,c.laplace_all,c.depth);
      Laplacian(c.blue_channel,c.laplace_blue,c.depth);
      Laplacian(c.green_channel,c.laplace_green,c.depth);
      Laplacian(c.red_channel,c.laplace_red,c.depth);


      // IMPORTANT ---- IF IMAGE IS CONVERTED TO 8UC3 and already is 8UC3 it is   <------------------------ FIX
      // NOT going to work.
}

void imgutil::fourier_transform(cvcontainer &c) {
    // forward dft calculations
    dft(c.gray_channel_32F,c.fourier_gray, CV_DXT_FORWARD);
    dft(c.blue_channel_32F,c.fourier_blue, CV_DXT_FORWARD);
    dft(c.green_channel_32F,c.fourier_green, CV_DXT_FORWARD);
    dft(c.red_channel_32F,c.fourier_red, CV_DXT_FORWARD);
}

// takes mean and median of the image channels
void imgutil::analyze_colors(cvcontainer &c) {

    c.mean_image = mean(c.data);
    c.mean_blue = c.mean_image.val[BLUE_LAYER];
    c.mean_green = c.mean_image.val[GREEN_LAYER];
    c.mean_red = c.mean_image.val[RED_LAYER];

    get_medians(c);
}

void imgutil::sumLaplace(cvcontainer &c) {
    // convert to 8bit image if not already
    if (c.laplace_all.depth() != CV_8U) {
        c.laplace_all.convertTo(c.laplace_all,CV_8U);
    }

    Scalar sumLaplace = sum(c.laplace_all);
    c.sumLaplace_blue = sumLaplace.val[BLUE_LAYER];
    c.sumLaplace_green = sumLaplace.val[GREEN_LAYER];
    c.sumLaplace_red = sumLaplace.val[RED_LAYER];
    c.sumLaplace_all = c.sumLaplace_blue + c.sumLaplace_green + c.sumLaplace_red;

}

void imgutil::sumCanny(cvcontainer &c){
    // create matrices of same size and depth
    Mat canny_all(c.height, c.width, SCC);
    Mat canny_blue(c.height, c.width, SCC);
    Mat canny_green(c.height, c.width, SCC);
    Mat canny_red(c.height, c.width, SCC);

    const int threshold_a = 1;
    int threshold_b;
    for (int i = 0; i <= 26; i++) {
        if (i == 0) {
            threshold_b = 1;
        }
        else if (i == 26) {
            threshold_b = 255;
        }
        else {
            threshold_b = 10*i;
        }

        // perform Canny transformations
        Canny(c.gray_channel, canny_all, threshold_a, threshold_b);
        Canny(c.blue_channel, canny_blue, threshold_a, threshold_b);
        Canny(c.green_channel, canny_green, threshold_a, threshold_b);
        Canny(c.red_channel, canny_red, threshold_b, threshold_b);

        // sum Canny matrices
        Scalar all_sum, blue_sum, green_sum, red_sum;
        all_sum = sum(canny_all);
        blue_sum = sum(canny_blue);
        green_sum = sum(canny_green);
        red_sum = sum(canny_red);

        c.sumCanny_all.push_back(all_sum.val[0]);
        c.sumCanny_blue.push_back(blue_sum.val[0]);
        c.sumCanny_green.push_back(green_sum.val[0]);
        c.sumCanny_red.push_back(red_sum.val[0]);
    }
}

void imgutil::sumBinLaplace(cvcontainer &c) {
    // create necessary matrices
    Mat binLaplace_blue(c.height,c.width,SCC);
    Mat binLaplace_green(c.height,c.width,SCC);
    Mat binLaplace_red(c.height,c.width,SCC);

    const double MAX_THRESHOLD = 255;
    double threshold;

    for (int i =0; i <= 26; i++) {
        if (i == 0) {
            threshold = 1;
        }
        else if (i == 26) {
            threshold = 255;
        }
        else {
            threshold = 10*i;
        }

        // thresholding layers
        cv::threshold(c.laplace_blue, binLaplace_blue, threshold, MAX_THRESHOLD, THRESH_BINARY);
        cv::threshold(c.laplace_green, binLaplace_blue, threshold, MAX_THRESHOLD, THRESH_BINARY);
        cv::threshold(c.laplace_red, binLaplace_blue, threshold, MAX_THRESHOLD, THRESH_BINARY);

        // sum matrices
        Scalar blue_sum, green_sum, red_sum;
        blue_sum = sum(binLaplace_blue);
        green_sum = sum(binLaplace_green);
        red_sum = sum(binLaplace_red);

        c.sumBinLaplace_blue.push_back(blue_sum.val[0]);
        c.sumBinLaplace_green.push_back(green_sum.val[0]);
        c.sumBinLaplace_red.push_back(red_sum.val[0]);
    }
}


/****** UTILITY PRIVATE METHODS *******
 **************************************/

void imgutil::get_medians(cvcontainer &c) {

    int size = c.data.total();
    // create intensity vectors of correct size
    vector<uchar> blue_intensities;
    blue_intensities.reserve(size);
    vector<uchar> green_intensities(blue_intensities);
    vector<uchar> red_intensities(green_intensities);

    int i=0;
    for (int y=0; y < c.data.rows; y++) {
        for (int x=0; x < c.data.cols; x++) {
            Scalar intensity = c.blue_channel.at<uchar>(y, x);
            blue_intensities.push_back(intensity.val[0]);
            intensity = c.green_channel.at<uchar>(y, x);
            green_intensities.push_back(intensity.val[0]);
            intensity = c.red_channel.at<uchar>(y, x);
            red_intensities.push_back(intensity.val[0]);
            i++;
        }
    }

    // get midpoint target
    int target = size/2;
    // partially sort array, using mid for nth element
    std::nth_element(blue_intensities.begin(),blue_intensities.begin()+target,blue_intensities.end());
    std::nth_element(green_intensities.begin(),green_intensities.begin()+target,green_intensities.end());
    std::nth_element(red_intensities.begin(),red_intensities.begin()+target,red_intensities.end());

    if (size % 2 == 1) {    // ODD NO. of PIXELS
        c.median_blue = blue_intensities.at(target);
        c.median_green = green_intensities.at(target);
        c.median_red = red_intensities.at(target);
    }
    else {
        int target_neighbor = target-1;
        std::nth_element(blue_intensities.begin(),blue_intensities.begin()+target_neighbor,blue_intensities.end());
        std::nth_element(green_intensities.begin(),green_intensities.begin()+target_neighbor,green_intensities.end());
        std::nth_element(red_intensities.begin(),red_intensities.begin()+target_neighbor,red_intensities.end());
        c.median_blue = (blue_intensities.at(target) + blue_intensities.at(target_neighbor)) / 2.0;
        c.median_green = (green_intensities.at(target) + green_intensities.at(target_neighbor)) / 2.0;
        c.median_red = (red_intensities.at(target) + red_intensities.at(target_neighbor)) / 2.0;
    }
}

// input is the container to be normalized
void imgutil::normalize(cvcontainer &in) {
    for (int y=0; y < in.data.rows; y++) {
        for (int x=0; x < in.data.cols; x++) {
            Vec3b intensity = in.data.at<Vec3b>(y, x);
            float blue = intensity.val[0];
            float green = intensity.val[1];
            float red = intensity.val[2];
            float sumbgr = blue + green + red;
            blue = (blue/sumbgr)*255;
            green = (green/sumbgr) * 255;
            red = (red/sumbgr) * 255;
            intensity.val[0] = blue;
            intensity.val[1] = green;
            intensity.val[2] = red;
            in.data.at<Vec3b>(x,y) = intensity;
        }
    }
}

// helper function to see depth type of image
string imgutil::get_depth(int depth) {
    switch (depth) {
    case 0:
        return "CV_8U";
        break;
    case 1:
        return "CV_8S";
        break;
    case 2:
        return "CV_16U";
        break;
    case 3:
        return "CV_16S";
        break;
    case 4:
        return "CV_32S";
        break;
    case 5:
        return "CV_32F";
        break;
    case 6:
        return "CV_64F";
        break;
    default:
        return "ERROR NO TYPE FOUND";
    }
}

// performs sanity checks on all files inputed
void imgutil::is_workable(cvcontainer &c) {
    if (c.dimension != 2) {   // image must be 2D
        cerr << "ERROR: not a workable image file [bad dimensions]" << endl;
        exit(EXIT_FAILURE);
    }
    if (c.depth < 0 || image.depth() > 6) {   // not valid image type
        cerr << "ERROR: not a workable image file [incorrect depth]" << endl;
        exit(EXIT_FAILURE);
    }
}

void imgutil::get_info(cvcontainer &c) {
    cout << "height: " << c.height << endl;
    cout << "width: " << c.width << endl;
    cout << "dimension: " << c.dimension << endl;
    cout << "depth: " << get_depth(c.depth) << endl;
    cout << "type: " << c.type << endl;
    cout << "channels: " << c.channels << endl;
}

void imgutil::get_info(Mat &m) {
    cout << "height: " << m.rows << endl;
    cout << "width: " << m.cols << endl;
    cout << "dimension: " << m.dims << endl;
    cout << "depth: " << get_depth(m.depth()) << endl;
    cout << "type: " << m.type() << endl;
    cout << "channels: " << m.channels() << endl;
}

void imgutil::show_image(Mat &image) {
    cvNamedWindow("coralysis", CV_WINDOW_AUTOSIZE);
    imshow("coralysis", image);
    waitKey(0);
}
