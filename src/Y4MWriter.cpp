//
// Created by JPe_D on 11/25/2023.
//

#include <iostream>
#include <fstream>
#include "Y4MWriter.h"

Y4MWriter::Y4MWriter(const string &out_fname) : outFile(out_fname, std::ios::binary) {}


void Y4MWriter::writeFrame(const cv::Mat* frame) {
    int width = frame->cols;
    int height = frame->rows;
    cv::Mat realframe;
    cv::cvtColor(*frame, realframe, cv::COLOR_BGR2YUV);
    // Open the Y4M file for writing
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open the file for writing." << std::endl;
        return;
    }

    // Write frame separator
    outFile << "FRAME" << std::endl;

    vector<cv::Mat> channels; 
    cv::split(realframe,channels);

    // Write Y4M frame
    outFile.write(reinterpret_cast<const char*>(channels[0].data), width * height);
    outFile.write(reinterpret_cast<const char*>(channels[1].data), width * height);
    outFile.write(reinterpret_cast<const char*>(channels[2].data), width * height);

}


void Y4MWriter::writeHeader(int width, int height, int fps) {
    // Open the Y4M file for writing
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open the file for writing." << std::endl;
        return;
    }
    // Write Y4M header
    outFile << "YUV4MPEG2 W" << width << " H" << height << " F" << fps << ":1 Ip A1:1 C444" << std::endl;
}

void Y4MWriter::closeFile() {
    outFile.close();
}

