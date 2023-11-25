//
// Created by JPe_D on 11/2/2023.
//

#include "Encoder.h"
#include <vector>
using namespace std;

Encoder::Encoder(VideoManipulator* in, BitStreamWrite* out) {
    this->m = 3; // initialize 'm'
    this->video = in;
    this->stream_out = out;
}

// TODO: falta o "video_format"
void Encoder::generate_headers(const Size& frame_size) {
    stream_out->write(to_string(frame_size.width));
    stream_out->write(to_string(frame_size.height)); // 2 bytes
    stream_out->write(to_string(video->getFPS()));

}

// Receives multi-channel Mat
void Encoder::encodeFrame(const Mat& f) {
    std::vector<Mat> channels;
    split(f, channels);
    for (int i = 0; i < f.channels(); ++i) {
        //this->m = GolombCode::estimate(channels[i]);
        stream_out->write(to_string(m));
        encodeChannel(channels[i]);
    }
}

// Receives single channel Mat
void Encoder::encodeChannel(const Mat& channel) {
    Mat estimate_mat = Mat::zeros(channel.cols - 1, channel.rows - 1, CV_8UC1);
    // encode first row and first column directly
    for (int col = 0; col < channel.cols; col++) {
        //stream_out->write(8, channel.at<uchar>(0, col));
        encodeValue(channel.at<uchar>(0, col));
    }

    for (int row = 1; row < channel.rows; row++) {
        //stream_out->write(8, channel.at<uchar>(row, 0));
        encodeValue(channel.at<uchar>(row, 0));

    }

    int r; // tem de ser int, pois pode ser negativo
    unsigned char a, b, c, p;

    for (int row = 1; row < channel.rows; row++)
        for (int col = 1; col < channel.cols; col++) {
            a = channel.at<uchar>(row, col - 1);
            b = channel.at<uchar>(row - 1, col);
            c = channel.at<uchar>(row - 1, col - 1);
            p = JPEG_LS(a, b, c);
            r = int(channel.at<uchar>(row, col)) - int(p);
            estimate_mat.at<uchar>(row, col) = GolombCode::mapIntToUInt(r);
            //encodeValue(GolombCode::mapIntToUInt(r));
        }

    m = GolombCode::estimate(estimate_mat);

    for (int row = 0; row < channel.rows; row++)
        for (int col = 0; col < channel.cols; col++) {
            encodeValue(estimate_mat.at<uchar>(row, col));
        }

}

void Encoder::encodeValue(unsigned int v) {
    GolombCode::encode(v, this->m, *stream_out);
}

void Encoder::encode() {
    Mat curr_frame;
    curr_frame = video->getNextFrame();
    generate_headers(curr_frame.size());
    cout << "encoding video..." << endl;
    imshow("first frame", curr_frame);
    waitKey(0);

    // frame loop
    int counter = 0; // testing stuff
    while (!curr_frame.empty()) {
        cout << "current_frame: " << counter << endl;
        encodeFrame(curr_frame);
        curr_frame = video->getNextFrame();
        counter++;
    }
}

unsigned char Encoder::JPEG_LS(unsigned char a, unsigned char b, unsigned char c) {
    unsigned char maximum = max(a,b);
    unsigned char minimum = min(a,b);
    if(c >= maximum)
        return minimum;
    else if (c <= minimum)
        return maximum;
    else
        return a + b - c;
}


