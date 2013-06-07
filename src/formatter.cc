/*  filename:   formatter.cc
 *  author:     David M. Westerhoff
 *  version:    alpha
 *  last mod:   4/17/13
 *  descript:   formatter class implementation that handles the column
 *              formating and variables from the image utility class
 */

#include "formatter.h"

// ctor has two parameters, the first imgutil to be formatted and the
// file name for the output stream
formatter::formatter(imgutil &iu, std::string filename) {
    output.open(filename.c_str());
    get_labels(iu);
    set_labels();
    output << std::endl;
    set_stats(iu);
}

// appends another files data to the output file
void formatter::append(imgutil &iu) {
    set_stats(iu);
}

// closes file output stream - prints newline at EOF
void formatter::close() {
    output.close();
}

// sets labels for base and normalized image variables
void formatter::set_labels() {
    std::vector<std::pair<std::string,int> >::iterator iter = labels.begin();
    while (iter != labels.end()) {
        output << "base-" << iter->first << "\t";
        iter++;
    }
    iter = labels.begin();
    while (iter != labels.end()) {
        output << "norm-" << iter->first << "\t";
        iter++;
    }
}

// gets all available labels for the image set
void formatter::get_labels(imgutil &iu){
    std::pair<std::string,int> temp;

    if(iu.doColors == true) {
        temp = std::make_pair("mean_blue",0);
        labels.push_back(temp);
        temp = std::make_pair("mean_green",0);
        labels.push_back(temp);
        temp = std::make_pair("mean_red",0);
        labels.push_back(temp);
        temp = std::make_pair("median_blue",0);
        labels.push_back(temp);
        temp = std::make_pair("median_green",0);
        labels.push_back(temp);
        temp = std::make_pair("median_red",0);
        labels.push_back(temp);
    }

    if(iu.doSumLaplace == true) {
        temp = std::make_pair("sumLaplace_all",0);
        labels.push_back(temp);
        temp = std::make_pair("sumLaplace_blue",0);
        labels.push_back(temp);
        temp = std::make_pair("sumLaplace_green",0);
        labels.push_back(temp);
        temp = std::make_pair("sumLaplace_red",0);
        labels.push_back(temp);
    }

    if(iu.doSumCanny == true) {
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            temp = std::make_pair("sumCanny_all",threshold_level);
            labels.push_back(temp);
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            temp = std::make_pair("sumCanny_blue",threshold_level);
            labels.push_back(temp);
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            temp = std::make_pair("sumCanny_green",threshold_level);
            labels.push_back(temp);
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            temp = std::make_pair("sumCanny_red",threshold_level);
            labels.push_back(temp);
        }
    }

    if(iu.doSumBinLaplace == true) {
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            temp = std::make_pair("sumBinLaplace_blue",threshold_level);
            labels.push_back(temp);
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            temp = std::make_pair("sumBinLaplace_green",threshold_level);
            labels.push_back(temp);
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            temp = std::make_pair("sumBinLaplace_red",threshold_level);
            labels.push_back(temp);
        }
    }
}

void formatter::set_stats(imgutil &iu) {
    output << iu.name << "\t";  //prints filename stats relate to

    // BASE IMAGE
    if(iu.doColors == true) {
        output << iu.base.mean_blue << "\t";
        output << iu.base.mean_green << "\t";
        output << iu.base.mean_red << "\t";
        output << iu.base.median_blue << "\t";
        output << iu.base.median_green << "\t";
        output << iu.base.median_red << "\t";
    }

    if(iu.doSumLaplace == true) {
        output << iu.base.sumLaplace_all << "\t";
        output << iu.base.sumLaplace_blue << "\t";
        output << iu.base.sumLaplace_green << "\t";
        output << iu.base.sumLaplace_red << "\t";
    }

    if(iu.doSumCanny == true) {
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.base.sumCanny_all.at(threshold_level) << "\t";
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.base.sumCanny_blue.at(threshold_level) << "\t";
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.base.sumCanny_green.at(threshold_level) << "\t";
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.base.sumCanny_red.at(threshold_level) << "\t";
        }
    }

    if(iu.doSumBinLaplace == true) {
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.base.sumBinLaplace_blue.at(threshold_level) << "\t";
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.base.sumBinLaplace_blue.at(threshold_level) << "\t";
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.base.sumBinLaplace_blue.at(threshold_level) << "\t";
        }
    }
    // NORMALIZED IMAGE
    if(iu.doColors == true) {
        output << iu.norm.mean_blue << "\t";
        output << iu.norm.mean_green << "\t";
        output << iu.norm.mean_red << "\t";
        output << iu.norm.median_blue << "\t";
        output << iu.norm.median_green << "\t";
        output << iu.norm.median_red << "\t";
    }

    if(iu.doSumLaplace == true) {
        output << iu.norm.sumLaplace_all << "\t";
        output << iu.norm.sumLaplace_blue << "\t";
        output << iu.norm.sumLaplace_green << "\t";
        output << iu.norm.sumLaplace_red << "\t";
    }

    if(iu.doSumCanny == true) {
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.norm.sumCanny_all.at(threshold_level) << "\t";
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.norm.sumCanny_blue.at(threshold_level) << "\t";
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.norm.sumCanny_green.at(threshold_level) << "\t";
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.norm.sumCanny_red.at(threshold_level) << "\t";
        }
    }

    if(iu.doSumBinLaplace == true) {
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.norm.sumBinLaplace_blue.at(threshold_level) << "\t";
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.norm.sumBinLaplace_blue.at(threshold_level) << "\t";
        }
        for (int threshold_level = 0; threshold_level<=26; threshold_level++) {
            output << iu.norm.sumBinLaplace_blue.at(threshold_level) << "\t";
        }
    }

    output << std::endl;    // end this file's stats with a new line
}
