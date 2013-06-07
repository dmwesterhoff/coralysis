/*  filename:   formatter.h
 *  author:     David M. Westerhoff
 *  version:    alpha
 *  last mod:   4/17/13
 *  descript:   header file for formatter class which columnizes data
 *              and outputs to a file stream
 */

#ifndef FORMATTER_H_
#define FORMATTER_H_

#include "imgutil.h"
#include <string>
#include <vector>
#include <fstream>

class imgutil;  // forward declaration

class formatter {
public:
    formatter(imgutil &iu, std::string filename);
    void append(imgutil &iu);
    void close();
private:
    std::vector<std::pair<std::string,int> > labels;
    std::ofstream output;
    void get_labels(imgutil &iu);
    void set_labels();
    void set_stats(imgutil &iu);
};

#endif /* FORMATTER_H_ */
