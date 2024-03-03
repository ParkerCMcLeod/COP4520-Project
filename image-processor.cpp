#include <cstdint>
#include <cmath>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

const std::string Filename = "in/small-image.bmp"; // file path
const std::string GaussianBlurredOutputFilename = "out/gaussian-blurred-image.bmp"; // output file path
const std::string BoxBlurredOutputFilename = "out/box-blurred-image.bmp"; // output file path
const std::string MotionBlurredOutputFilename = "out/motion-blurred-image.bmp"; // output file path

const double Sigma = 3.0; // Gaussian blur sigma value (blur radius, significant performance impact) 
const int BoxSize = 9; // box blur value (blur radius, must be odd) 

struct RGB {
    uint8_t blue, green, red; // RGB structure with the color order as blue, green, red to allow bottom up parsing present in .bmp
};

const double PI = 3.14159265358979323846; // PI constant

// read bitmap images
std::vector<std::vector<RGB>> readBmp(const std::string& filename);
// generate the Gaussian kernel
std::vector<std::vector<double>> generateGaussianKernel(double sigma);
// apply Gaussian blur to the image
std::vector<std::vector<RGB>> applyGaussianBlur(const std::vector<std::vector<RGB>>& image, const std::vector<std::vector<double>>& kernel);
// save the new image data by copying the original file and replacing the pixel color data
void writeBmp(const std::string& filename, const std::vector<std::vector<RGB>>& image);
// apply box blur to the image
std::vector<std::vector<RGB>> applyBoxBlur(const std::vector<std::vector<RGB>>& image, int boxSize);

int main() {
    auto start = std::chrono::high_resolution_clock::now(); // start timing
    std::cout << std::endl << "Parsing input image..." << std::endl;
    auto image = readBmp(Filename); // read the image from file
    auto end = std::chrono::high_resolution_clock::now(); // end timing
    std::chrono::duration<double> elapsed = end - start; // calculate elapsed time
    std::cout << "Time taken for parsing input image (" << (image[0].size() * image.size()) << "px): " << elapsed.count() << " seconds." << std::endl << std::endl;

    std::cout << "Applying Gaussian blur (Sigma=" << Sigma << ")..." << std::endl;
    start = std::chrono::high_resolution_clock::now(); // reset start time
    auto kernel = generateGaussianKernel(Sigma); // generate the Gaussian kernel (precompute the blur matrix values)
    auto blurredImage = applyGaussianBlur(image, kernel); // apply Gaussian blur to the image
    end = std::chrono::high_resolution_clock::now(); // end timing
    elapsed = end - start; // calculate elapsed time
    std::cout << "Time taken for applying Gaussian blur: " << elapsed.count() << " seconds." << std::endl;
    writeBmp(GaussianBlurredOutputFilename, blurredImage); // write the blurred image to a new file
    std::cout << "Saved gaussian blurred image to \"" << GaussianBlurredOutputFilename << "\"" << std::endl << std::endl;

    std::cout << "Applying box blur (BoxSize=" << BoxSize << ")..." << std::endl;
    start = std::chrono::high_resolution_clock::now(); // reset start time
    auto boxBlurredImage = applyBoxBlur(image, BoxSize);
    end = std::chrono::high_resolution_clock::now(); // end timing
    elapsed = end - start; // calculate elapsed time
    std::cout << "Time taken for applying box blur: " << elapsed.count() << " seconds." << std::endl;
    writeBmp(BoxBlurredOutputFilename, boxBlurredImage); // write the blurred image to a new file
    std::cout << "Saved box-blurred image to \"" << BoxBlurredOutputFilename << "\"" << std::endl << std::endl;

    return 0;
}

// read bitmap images
std::vector<std::vector<RGB>> readBmp(const std::string& filename) {
    std::ifstream bmpFile(filename, std::ios::binary); // open the BMP file in binary mode
    std::vector<std::vector<RGB>> image; // create a 2D vector to store the pixels
    if (!bmpFile) {
        std::cerr << "Could not open BMP file!" << std::endl; // check if the file was successfully opened
        return image;
    }

    int32_t width, height; // declare variables to store the width and height of the image
    bmpFile.seekg(18); // seek to the width and height in the BMP header
    bmpFile.read(reinterpret_cast<char*>(&width), sizeof(width)); // read the width
    bmpFile.read(reinterpret_cast<char*>(&height), sizeof(height)); // read the height

    image.resize(height, std::vector<RGB>(width)); // resize the vector to fit the image dimensions
    int rowPadding = (4 - (width * 3) % 4) % 4; // calculate the row padding
    bmpFile.seekg(54); // seek to the start of the image data
    for (int y = height - 1; y >= 0; y--) {
        bmpFile.read(reinterpret_cast<char*>(image[y].data()), width * sizeof(RGB)); // read each row of the image
        bmpFile.ignore(rowPadding); // ignore the row padding
    }

    return image; // return the read image
}

// generate the Gaussian kernel
std::vector<std::vector<double>> generateGaussianKernel(double sigma) {
    int kernelSize = static_cast<int>(std::round(6 * sigma)) | 1; // calculate the kernel size (guarantee odd for central pixel)
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize)); // create a 2D vector for the kernel
    double sum = 0.0; // store the sum of all elements in the kernel
    int halfSize = kernelSize / 2; // calculate the half size of the kernel

    for (int x = -halfSize; x <= halfSize; x++) {
        for (int y = -halfSize; y <= halfSize; y++) {
            double exponent = -(x * x + y * y) / (2 * sigma * sigma); // calculate the exponent
            kernel[x + halfSize][y + halfSize] = std::exp(exponent) / (2 * PI * sigma * sigma); // calculate each kernel element
            sum += kernel[x + halfSize][y + halfSize]; // add the element to the sum
        }
    }

    // normalize the kernel
    for (auto& row : kernel) {
        for (double& value : row) {
            value /= sum; // divide each element by the sum to normalize
        }
    }

    return kernel; // return the generated kernel
}

// apply Gaussian blur to an image
std::vector<std::vector<RGB>> applyGaussianBlur(const std::vector<std::vector<RGB>>& image, const std::vector<std::vector<double>>& kernel) {
    int height = image.size(), width = image[0].size(), kernelSize = kernel.size(); // get the dimensions of the image and the kernel
    std::vector<std::vector<RGB>> blurredImage(height, std::vector<RGB>(width)); // create a 2D vector for the blurred image

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double totalRed = 0, totalGreen = 0, totalBlue = 0; // variables to store the total color values
            for (int ky = -kernelSize / 2; ky <= kernelSize / 2; ++ky) {
                for (int kx = -kernelSize / 2; kx <= kernelSize / 2; ++kx) {
                    int pixelPosX = x + kx, pixelPosY = y + ky; // calculate the position of the current pixel
                    if (pixelPosX >= 0 && pixelPosX < width && pixelPosY >= 0 && pixelPosY < height) { // check if the position is within the image boundaries
                        const auto& pixel = image[pixelPosY][pixelPosX]; // get the current pixel
                        double kernelValue = kernel[ky + kernelSize / 2][kx + kernelSize / 2]; // get the corresponding kernel value
                        totalRed += pixel.red * kernelValue; // multiply the pixel's red value by the kernel value and add to the total
                        totalGreen += pixel.green * kernelValue; // multiply the pixel's green value by the kernel value and add to the total
                        totalBlue += pixel.blue * kernelValue; // multiply the pixel's blue value by the kernel value and add to the total
                    }
                }
            }
            // calculate the average for each color channel and clamp the values to the valid range [0, 255]
            blurredImage[y][x].red = std::clamp(static_cast<int>(totalRed), 0, 255); // set the red value of the blurred pixel
            blurredImage[y][x].green = std::clamp(static_cast<int>(totalGreen), 0, 255); // set the green value of the blurred pixel
            blurredImage[y][x].blue = std::clamp(static_cast<int>(totalBlue), 0, 255); // set the blue value of the blurred pixel
        }
    }

    return blurredImage; // return the blurred image
}

// save the new image data by copying the original file and replacing the pixel color data
void writeBmp(const std::string& filename, const std::vector<std::vector<RGB>>& image) {
    std::ifstream bmpFile(Filename, std::ios::binary); // open the original BMP file in binary mode to read the header
    std::ofstream outFile(filename, std::ios::binary); // open the output file in binary mode

    if (!bmpFile || !outFile) {
        std::cerr << "Could not open BMP files for reading/writing." << std::endl;
        return;
    }

    // copy the header from the original BMP file
    std::vector<char> header(54);
    bmpFile.read(header.data(), 54);
    outFile.write(header.data(), 54);

    int width = image[0].size(), height = image.size();
    int rowPadding = (4 - (width * 3) % 4) % 4;

    // write the new image data
    for (int y = height - 1; y >= 0; y--) {
        outFile.write(reinterpret_cast<const char*>(image[y].data()), width * sizeof(RGB));
        for (int i = 0; i < rowPadding; i++) {
            outFile.put(0);
        }
    }
}

// apply box blur to the image
std::vector<std::vector<RGB>> applyBoxBlur(const std::vector<std::vector<RGB>>& image, int boxSize) {
    int height = image.size(), width = image[0].size(); // dimensions of the input image
    std::vector<std::vector<RGB>> blurredImage(height, std::vector<RGB>(width)); // preparing the output image with the same dimensions
    int halfBoxSize = boxSize / 2; // computing half the box size to use as an offset around each pixel

    // iterate through each pixel in the image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int count = 0; // counter for the number of pixels considered in the box
            double totalRed = 0, totalGreen = 0, totalBlue = 0; // accumulators for the color channels

            // iterate over the box surrounding the current pixel
            for (int dy = -halfBoxSize; dy <= halfBoxSize; ++dy) {
                for (int dx = -halfBoxSize; dx <= halfBoxSize; ++dx) {
                    int newY = y + dy, newX = x + dx; // calculate the position of the neighboring pixel

                    // check if the neighboring pixel is within the image bounds
                    if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                        const auto& pixel = image[newY][newX]; // access the neighboring pixel
                        totalRed += pixel.red; // accumulate the red color channel
                        totalGreen += pixel.green; // accumulate the green color channel
                        totalBlue += pixel.blue; // accumulate the blue color channel
                        ++count; // increment the counter
                    }
                }
            }

            // calculate the average for each color channel and clamp the values to the valid range [0, 255]
            blurredImage[y][x].red = std::clamp(static_cast<int>(totalRed / count), 0, 255);
            blurredImage[y][x].green = std::clamp(static_cast<int>(totalGreen / count), 0, 255);
            blurredImage[y][x].blue = std::clamp(static_cast<int>(totalBlue / count), 0, 255);
        }
    }

    return blurredImage; // return the blurred image
}