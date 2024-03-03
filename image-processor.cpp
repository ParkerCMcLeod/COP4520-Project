#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

const std::string Filename = "small-image.bmp"; // file path
const double Sigma = 1.0; // Gaussian blur sigma value (blur radius)

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

int main() {
    auto image = readBmp(Filename); // read the image from file

    // print RGB values of the first pixel in the original image
    const auto& pixelOriginal = image[0][0];
    std::cout << "RGB of the first pixel in original image: "
                << "R=" << static_cast<int>(pixelOriginal.red) << ", "
                << "G=" << static_cast<int>(pixelOriginal.green) << ", "
                << "B=" << static_cast<int>(pixelOriginal.blue) << std::endl;

    auto kernel = generateGaussianKernel(Sigma); // generate the Gaussian kernel (precompute the blur matrix values)
    auto blurredImage = applyGaussianBlur(image, kernel); // apply Gaussian blur to the image

    // print RGB values of the first pixel in the blurred image
    const auto& pixelBlurred = blurredImage[0][0];
    std::cout << "RGB of the first pixel in blurred image: "
                << "R=" << static_cast<int>(pixelBlurred.red) << ", "
                << "G=" << static_cast<int>(pixelBlurred.green) << ", "
                << "B=" << static_cast<int>(pixelBlurred.blue) << std::endl;

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
            blurredImage[y][x].red = std::clamp(static_cast<int>(totalRed), 0, 255); // set the red value of the blurred pixel
            blurredImage[y][x].green = std::clamp(static_cast<int>(totalGreen), 0, 255); // set the green value of the blurred pixel
            blurredImage[y][x].blue = std::clamp(static_cast<int>(totalBlue), 0, 255); // set the blue value of the blurred pixel
        }
    }

    return blurredImage; // return the blurred image
}
