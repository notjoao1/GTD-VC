#ifndef GTD_VC_DECODER_H
#define GTD_VC_DECODER_H

#include "BitStream/BitStreamRead.h"
#include "Encoding/GolombCode.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include "Y4MWriter.h"

using namespace cv;
using namespace std;

/**
 * @brief Class for decoding ( only intra frame ).
 */
class Decoder {
private:
    BitStreamRead stream_in; /**< Pointer to the BitStreamRead instance for input stream. */
    int m; /**< Golomb parameter. */
    int cols; /**< Number of columns in the video frames. */
    int rows; /**< Number of rows in the video frames. */
    int fps_num; /**< Frames per second numerator. */
    int fps_denominator; /**< Frames per second denominator. */
    int quantizationY; /**< Quantization level for luma channel for lossy encoding. */
    int quantizationU; /**< Quantization level for U chroma channel for lossy encoding. */
    int quantizationV; /**< Quantization level for V chroma channel for lossy encoding. */
public:
    Y4MWriter output_vid; /**< Y4MWriter instance for writing the decoded video. */
    /**
     * @brief Reads and processes headers from the input stream.
     */
    void read_headers();

    /**
     * @brief Decodes a single frame from the input stream.
     * @return The decoded frame.
     */
    Mat decodeFrame();

    /**
     * @brief Decodes a single channel from the input stream.
     * @return The decoded channel.
     */
    Mat decodeChannel(int quantization);

    /**
     * @brief Decodes a single value using Golomb decoding.
     * @return The decoded value.
     */
    int decodeValue();

    /**
     * @brief Performs JPEG-LS decoding on three input bytes.
     * @param a First byte.
     * @param b Second byte.
     * @param c Third byte.
     * @return The output value from JPEG-LS.
     */
    static unsigned char JPEG_LS(unsigned char a, unsigned char b, unsigned char c);

    /**
     * @brief Constructor for the Decoder class.
     * @param input_fname The input file encoded using our encoder, should be in .gtd format.
     * @param output_fname The output file name for writing the decoded video.
     */
    Decoder(const std::string& input_fname, const std::string& output_fname);

    /**
     * @brief Main decoding function that decodes the entire video.
     */
    void decode();

};

#endif // GTD_VC_DECODER_H