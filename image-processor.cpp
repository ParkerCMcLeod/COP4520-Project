#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>


std::string InputFilename; // Input
const std::string GaussianBlurredOutputFilename = "out/gaussianBlur.bmp"; // Output
const std::string BoxBlurredOutputFilename = "out/boxBlur.bmp"; // Output
const std::string MotionBlurredOutputFilename = "out/motionBlur.bmp"; // Output
const std::string BucketFillOutputFilename = "out/bucketFill.bmp"; // Output
const std::string BilinearResizedOutputFilename = "out/bilinearResize.bmp"; // Output
const std::string BicubicResizedOutputFilename = "out/bicubicResize.bmp"; // Output
const std::string nearestNeighborResizedOutputFilename = "out/nearestNeighborResize.bmp"; // Output

double sigma = 3.0; // Gaussian blur sigma value (blur radius, significant performance impact)
int boxSize = 9; // Box blur value (blur radius, must be odd)
int motionLength = 15; // Define the length of the motion blur
int bucketFillThreshold = 75; // Threshold for bucket fill
int bucketFillX = 800; // X pixel location for starting bucket fill
int bucketFillY = 170; // Y pixel location for starting bucket fill
int resizeWidthBilinear = 500; // Desired resize width
int resizeHeightBilinear = 745; // Desired resize height
int resizeWidthBicubic = 500; // Desired resize width
int resizeHeightBicubic = 745; // Desired resize height
int resizeWidthNearestNeighbor = 500; // Desired resize width
int resizeHeightNearestNeighbor = 745; // Desired resize height
std::string inputImageSize = "small"; // Which input image to use (small medium large)
std::string function = "all"; // Which function to run (all gaussianBlur boxBlur motionBlur bucketFill bilinearResize bicubicResize nearestNeighborResize)

constexpr double PI = 3.14159265358979323846; // PI constant


// RGB structure with the color order as blue, green, red to allow bottom up parsing present in .bmp
struct RGB {
    uint8_t blue, green, red; 
};


// Create an out folder
void createOutFolder();

// Helper function for parsing image
std::vector<std::vector<RGB>> parseImageHelper();

// Helper function for timing and implementing the gaussian blur function
void gaussianBlurHelper(std::vector<std::vector<RGB>> image);
// Helper function for timing and implementing the box blur function
void boxBlurHelper(std::vector<std::vector<RGB>> image);
// Helper function for timing and implementing the motion blur function
void motionBlurHelper(std::vector<std::vector<RGB>> image);
// Helper function for timing and implementing the bucket fill function
void bucketFillHelper(std::vector<std::vector<RGB>> image);
// Helper function for timing and implementing the bilinear resize function
void bilinearResizeHelper(std::vector<std::vector<RGB>> image);
// Helper function for timing and implementing the bicubic resize function
void bicubicResizeHelper(std::vector<std::vector<RGB>> image);
// Helper function for timing and implementing the nearest neighbor resize function
void nearestNeighborResizeHelper(std::vector<std::vector<RGB>> image);

// Read bitmap images with one thread
std::vector<std::vector<RGB>> readBmpSingleThread(const std::string& filename);
// Generate the Gaussian kernel with one thread
std::vector<std::vector<double>> generateGaussianKernelSingleThread(double sigma);
// Apply Gaussian blur to an image with one thread
std::vector<std::vector<RGB>> applyGaussianBlurSingleThread(const std::vector<std::vector<RGB>>& image, const std::vector<std::vector<double>>& kernel);
// Apply box blur to the image with one thread
std::vector<std::vector<RGB>> applyBoxBlurSingleThread(const std::vector<std::vector<RGB>>& image, int boxSize);
// Apply motion blur to the image based on a given motion length with one thread
std::vector<std::vector<RGB>> applyMotionBlurSingleThread(const std::vector<std::vector<RGB>>& image, int motionLength);
// Function to calculate Euclidean distance between two colors in RGB space with one thread
double colorDistanceSingleThread(const RGB& color1, const RGB& color2);
// Apply bucket fill to the other image with one thread
std::vector<std::vector<RGB>> applyBucketFillSingleThread(const std::vector<std::vector<RGB>>& image, int threshold);
// Bicubic interpolation kernel based on Catmull-Rom spline with one thread
double cubicInterpolateSingleThread(double p[4], double x);
// Function to perform bicubic interpolation on a 4x4 patch of an image with one thread
double bicubicInterpolateSingleThread(double arr[4][4], double x, double y);
// Function to resize an image using bicubic interpolation with one thread
std::vector<std::vector<RGB>> resizeBicubicSingleThread(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight);
// Function to resize an image using bilinear interpolation with one thread
std::vector<std::vector<RGB>> resizeBilinearSingleThread(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight);
// Apply nearest neighbor resizing to the image  with one thread
std::vector<std::vector<RGB>> nearestNeighborResizeSingleThread(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight);
// Save the new image data by copying the original file and replacing the header (for resize) and pixel color data  with one thread
void writeBmpSingleThread(const std::string& filename, const std::vector<std::vector<RGB>>& image, bool resize, int resizedWidth=-1, int resizedHeight=-1);

// Read bitmap images with many threads
std::vector<std::vector<RGB>> readBmpMultipleThreads(const std::string& filename);
// Generate the Gaussian kernel with many threads
std::vector<std::vector<double>> generateGaussianKernelMultipleThreads(double sigma);
// Apply Gaussian blur to an image with many threads
std::vector<std::vector<RGB>> applyGaussianBlurMultipleThreads(const std::vector<std::vector<RGB>>& image, const std::vector<std::vector<double>>& kernel);
// Apply box blur to the image with many threads
std::vector<std::vector<RGB>> applyBoxBlurMultipleThreads(const std::vector<std::vector<RGB>>& image, int boxSize);
// Apply motion blur to the image based on a given motion length with many threads
std::vector<std::vector<RGB>> applyMotionBlurMultipleThreads(const std::vector<std::vector<RGB>>& image, int motionLength);
// Function to calculate Euclidean distance between two colors in RGB space with many threads
double colorDistanceMultipleThreads(const RGB& color1, const RGB& color2);
// Apply bucket fill to the other image with many threads
std::vector<std::vector<RGB>> applyBucketFillMultipleThreads(const std::vector<std::vector<RGB>>& image, int threshold);
// Bicubic interpolation kernel based on Catmull-Rom spline with many threads
double cubicInterpolateMultipleThreads(double p[4], double x);
// Function to perform bicubic interpolation on a 4x4 patch of an image with many threads
double bicubicInterpolateMultipleThreads(double arr[4][4], double x, double y);
// Function to resize an image using bicubic interpolation with many threads
std::vector<std::vector<RGB>> resizeBicubicMultipleThreads(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight);
// Function to resize an image using bilinear interpolation with many threads
std::vector<std::vector<RGB>> resizeBilinearMultipleThreads(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight);
// Apply nearest neighbor resizing to the image  with many threads
std::vector<std::vector<RGB>> nearestNeighborResizeMultipleThreads(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight);
// Save the new image data by copying the original file and replacing the header (for resize) and pixel color data  with many threads
void writeBmpMultipleThreads(const std::string& filename, const std::vector<std::vector<RGB>>& image, bool resize, int resizedWidth=-1, int resizedHeight=-1);


int main(int argc, char* argv[]) {
    if (argc < 15) {
        std::cerr << "Usage: " << argv[0] << " <sigma> <boxSize> <motionLength> <bucketFillThreshold> <bucketFillX> <bucketFillY> resizeWidthBilinear <resizeHeightBilinear> <resizeWidthBicubic> <resizeHeightBicubic> <resizeWidthNearestNeighbor> <resizeHeightNearestNeighbor> <inputImageSize> <function>" << std::endl << std::endl;
        return 1;
    }

    // Parse the command-line arguments
    sigma = std::atof(argv[1]);
    boxSize = std::atoi(argv[2]);
    motionLength = std::atoi(argv[3]);
    bucketFillThreshold = std::atoi(argv[4]);
    bucketFillX = std::atoi(argv[5]);
    bucketFillY = std::atoi(argv[6]);
    resizeWidthBilinear = std::atoi(argv[7]);
    resizeHeightBilinear = std::atoi(argv[8]);
    resizeWidthBicubic = std::atoi(argv[9]);
    resizeHeightBicubic = std::atoi(argv[10]);
    resizeWidthNearestNeighbor = std::atoi(argv[11]);
    resizeHeightNearestNeighbor = std::atoi(argv[12]);
    inputImageSize = argv[13];
    function = argv[14];

    if (inputImageSize == "small") {
        InputFilename = "in/smallImage.bmp";
    } else if (inputImageSize == "medium") {
        InputFilename = "in/mediumImage.bmp";
    } else if (inputImageSize == "large") {
        InputFilename = "in/largeImage.bmp";
    } else {
        std::cerr << "Unknown input image size: " << InputFilename << std::endl;
        return 1;
    }

    createOutFolder(); // Create an out folder
    auto image = parseImageHelper(); // Helper function for parsing image  

    // Map of functions to their respective handlers
    std::unordered_map<std::string, std::function<void(std::vector<std::vector<RGB>>&)> > functions = {
        {"gaussianBlur", gaussianBlurHelper},
        {"boxBlur", boxBlurHelper},
        {"motionBlur", motionBlurHelper},
        {"bucketFill", bucketFillHelper},
        {"bilinearResize", bilinearResizeHelper},
        {"bicubicResize", bicubicResizeHelper},
        {"nearestNeighborResize", nearestNeighborResizeHelper}
    };

    // Execute specified function if provided, else execute all
    if (function != "all") {
        if (functions.find(function) != functions.end()) {
            functions[function](image);
        } else {
            std::cerr << "Unknown function: " << function << std::endl;
            return 1;
        }
    } else {
        for (auto& func : functions) {
            func.second(image);
        }
    }

    return 0;
}

// Create an out folder
void createOutFolder() {
    const char* dir = "out";

#ifdef _WIN32
    if (!CreateDirectory(dir, NULL)) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            std::cout << "Directory already exists.\n";
        } else {
            std::cout << "Failed to create directory.\n";
        }
    } else {
        std::cout << "Directory created successfully.\n";
    }
#else
    struct stat st;
    if (stat(dir, &st) != 0) {
        if (mkdir(dir, 0755) == 0) {
            std::cout << "Directory created successfully.\n";
        } else {
            perror("Failed to create directory");
        }
    } else {
        std::cout << "Directory already exists.\n";
    }
#endif
}

std::vector<std::vector<RGB>> parseImageHelper() {
    std::cout << std::endl << "Parsing input image..." << std::endl << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto image = readBmpSingleThread(InputFilename);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken for parsing input image (" << (image[0].size() * image.size()) << "px): " << elapsed.count() << " milliseconds." << std::endl << std::endl;

    return image;
}

void gaussianBlurHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying Gaussian blur (sigma=" << sigma << ")..." << std::endl << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto kernel = generateGaussianKernelSingleThread(sigma);
    auto blurredImage = applyGaussianBlurSingleThread(image, kernel);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken for applying Gaussian blur: " << elapsed.count() << " milliseconds." << std::endl << std::endl;
    writeBmpSingleThread(GaussianBlurredOutputFilename, blurredImage, false);
    std::cout << "Saved gaussian blurred image to \"" << GaussianBlurredOutputFilename << "\"" << std::endl << std::endl;
}

void boxBlurHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying box blur (boxSize=" << boxSize << ")..." << std::endl << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto boxBlurredImage = applyBoxBlurSingleThread(image, boxSize);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken for applying box blur: " << elapsed.count() << " milliseconds." << std::endl << std::endl;
    writeBmpSingleThread(BoxBlurredOutputFilename, boxBlurredImage, false);
    std::cout << "Saved box-blurred image to \"" << BoxBlurredOutputFilename << "\"" << std::endl << std::endl;
}

void motionBlurHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying motion blur (motionLength=" << motionLength << ")..." << std::endl << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto motionBlurredImage = applyMotionBlurSingleThread(image, motionLength);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken for applying motion blur: " << elapsed.count() << " milliseconds." << std::endl << std::endl;
    writeBmpSingleThread(MotionBlurredOutputFilename, motionBlurredImage, false);
    std::cout << "Saved motion-blurred image to \"" << MotionBlurredOutputFilename << "\"" << std::endl << std::endl;
}

void bucketFillHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying bucket fill (Threshold=" << bucketFillThreshold << ")..." << std::endl << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto bucketFilledImage = applyBucketFillSingleThread(image, bucketFillThreshold);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken for applying bucket fill: " << elapsed.count() << " milliseconds." << std::endl << std::endl;
    writeBmpSingleThread(BucketFillOutputFilename, bucketFilledImage, false);
    std::cout << "Saved bucket-filled image to \"" << BucketFillOutputFilename << "\"" << std::endl << std::endl;
}

void bilinearResizeHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying bilinear resizing (Output Size=" << resizeWidthBilinear << "x" << resizeHeightBilinear << ")..." << std::endl << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto bilinearResizedImage = resizeBilinearSingleThread(image, resizeWidthBilinear, resizeHeightBilinear);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken for applying bilinear resizing: " << elapsed.count() << " milliseconds." << std::endl << std::endl;
    writeBmpSingleThread(BilinearResizedOutputFilename, bilinearResizedImage, true, resizeWidthBilinear, resizeHeightBilinear);
    std::cout << "Saved bilinear-resized image to \"" << BilinearResizedOutputFilename << "\"" << std::endl << std::endl;
}

void bicubicResizeHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying bicubic resizing (Output Size=" << resizeWidthBicubic << "x" << resizeHeightBicubic << ")..." << std::endl << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto bicubicResizedImage = resizeBicubicSingleThread(image, resizeWidthBicubic, resizeHeightBicubic);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken for applying bicubic resizing: " << elapsed.count() << " milliseconds." << std::endl << std::endl;
    writeBmpSingleThread(BicubicResizedOutputFilename, bicubicResizedImage, true, resizeWidthBicubic, resizeHeightBicubic);
    std::cout << "Saved bicubic-resized image to \"" << BicubicResizedOutputFilename << "\"" << std::endl << std::endl;
}

void nearestNeighborResizeHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying nearest neighbor resizing (Output Size=" << resizeWidthNearestNeighbor << "x" << resizeHeightNearestNeighbor << ")..." << std::endl << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto nearestNeighborResizeSingleThreaddImage = nearestNeighborResizeSingleThread(image, resizeWidthNearestNeighbor, resizeHeightNearestNeighbor);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken for applying nearest neighbor resizing: " << elapsed.count() << " milliseconds." << std::endl << std::endl;
    writeBmpSingleThread(nearestNeighborResizedOutputFilename, nearestNeighborResizeSingleThreaddImage, true, resizeWidthNearestNeighbor, resizeHeightNearestNeighbor);
    std::cout << "Saved nearestNeighbor-resized image to \"" << nearestNeighborResizedOutputFilename << "\"" << std::endl << std::endl;
}


// Read bitmap images with one thread
std::vector<std::vector<RGB>> readBmpSingleThread(const std::string& filename) {
    std::ifstream bmpFile(filename, std::ios::binary); // Open the BMP file in binary mode
    std::vector<std::vector<RGB>> image; // Create a 2D vector to store the pixels
    if (!bmpFile) {
        std::cerr << "Could not open BMP file!" << std::endl << std::endl; // Check if the file was successfully opened
        return image;
    }

    int32_t width, height; // Declare variables to store the width and height of the image
    bmpFile.seekg(18); // Seek to the width and height in the BMP header
    bmpFile.read(reinterpret_cast<char*>(&width), sizeof(width)); // Read the width
    bmpFile.read(reinterpret_cast<char*>(&height), sizeof(height)); // Read the height

    image.resize(height, std::vector<RGB>(width)); // Resize the vector to fit the image dimensions
    int rowPadding = (4 - (width * 3) % 4) % 4; // Calculate the row padding
    bmpFile.seekg(54); // Seek to the start of the image data
    for (int y = height - 1; y >= 0; y--) {
        bmpFile.read(reinterpret_cast<char*>(image[y].data()), width * sizeof(RGB)); // Read each row of the image
        bmpFile.ignore(rowPadding); // Ignore the row padding
    }

    return image; // Return the read image
}

// Generate the Gaussian kernel with one thread
std::vector<std::vector<double>> generateGaussianKernelSingleThread(double sigma) {
    int kernelSize = static_cast<int>(std::round(6 * sigma)) | 1; // Calculate the kernel size (guarantee odd for central pixel)
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize)); // Create a 2D vector for the kernel
    double sum = 0.0; // Store the sum of all elements in the kernel
    int halfSize = kernelSize / 2; // Calculate the half size of the kernel

    for (int x = -halfSize; x <= halfSize; x++) {
        for (int y = -halfSize; y <= halfSize; y++) {
            double exponent = -(x * x + y * y) / (2 * sigma * sigma); // Calculate the exponent
            kernel[x + halfSize][y + halfSize] = std::exp(exponent) / (2 * PI * sigma * sigma); // Calculate each kernel element
            sum += kernel[x + halfSize][y + halfSize]; // Add the element to the sum
        }
    }

    // Normalize the kernel
    for (auto& row : kernel) {
        for (double& value : row) {
            value /= sum; // Divide each element by the sum to normalize
        }
    }

    return kernel; // Return the generated kernel
}

// Apply Gaussian blur to an image with one thread
std::vector<std::vector<RGB>> applyGaussianBlurSingleThread(const std::vector<std::vector<RGB>>& image, const std::vector<std::vector<double>>& kernel) {
    int height = image.size(), width = image[0].size(), kernelSize = kernel.size(); // Get the dimensions of the image and the kernel
    std::vector<std::vector<RGB>> blurredImage(height, std::vector<RGB>(width)); // Create a 2D vector for the blurred image

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double totalRed = 0, totalGreen = 0, totalBlue = 0; // Variables to store the total color values
            for (int ky = -kernelSize / 2; ky <= kernelSize / 2; ++ky) {
                for (int kx = -kernelSize / 2; kx <= kernelSize / 2; ++kx) {
                    int pixelPosX = x + kx, pixelPosY = y + ky; // Calculate the position of the current pixel
                    if (pixelPosX >= 0 && pixelPosX < width && pixelPosY >= 0 && pixelPosY < height) { // Check if the position is within the image boundaries
                        const auto& pixel = image[pixelPosY][pixelPosX]; // Get the current pixel
                        double kernelValue = kernel[ky + kernelSize / 2][kx + kernelSize / 2]; // Get the corresponding kernel value
                        totalRed += pixel.red * kernelValue; // Multiply the pixel's red value by the kernel value and add to the total
                        totalGreen += pixel.green * kernelValue; // Multiply the pixel's green value by the kernel value and add to the total
                        totalBlue += pixel.blue * kernelValue; // Multiply the pixel's blue value by the kernel value and add to the total
                    }
                }
            }
            // Calculate the average for each color channel and clamp the values to the valid range [0, 255]
            blurredImage[y][x].red = std::clamp(static_cast<int>(totalRed), 0, 255); // Set the red value of the blurred pixel
            blurredImage[y][x].green = std::clamp(static_cast<int>(totalGreen), 0, 255); // Set the green value of the blurred pixel
            blurredImage[y][x].blue = std::clamp(static_cast<int>(totalBlue), 0, 255); // Set the blue value of the blurred pixel
        }
    }

    return blurredImage; // Return the blurred image
}

// Apply box blur to the image with one thread
std::vector<std::vector<RGB>> applyBoxBlurSingleThread(const std::vector<std::vector<RGB>>& image, int boxSize) {
    int height = image.size(), width = image[0].size(); // Dimensions of the input image
    std::vector<std::vector<RGB>> blurredImage(height, std::vector<RGB>(width)); // Preparing the output image with the same dimensions
    int halfBoxSize = boxSize / 2; // Computing half the box size to use as an offset around each pixel

    // Iterate through each pixel in the image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int count = 0; // Counter for the number of pixels considered in the box
            double totalRed = 0, totalGreen = 0, totalBlue = 0; // Accumulators for the color channels

            // Iterate over the box surrounding the current pixel
            for (int dy = -halfBoxSize; dy <= halfBoxSize; ++dy) {
                for (int dx = -halfBoxSize; dx <= halfBoxSize; ++dx) {
                    int newY = y + dy, newX = x + dx; // Calculate the position of the neighboring pixel

                    // Check if the neighboring pixel is within the image bounds
                    if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                        const auto& pixel = image[newY][newX]; // Access the neighboring pixel
                        totalRed += pixel.red; // Accumulate the red color channel
                        totalGreen += pixel.green; // Accumulate the green color channel
                        totalBlue += pixel.blue; // Accumulate the blue color channel
                        ++count; // Increment the counter
                    }
                }
            }

            // Calculate the average for each color channel and clamp the values to the valid range [0, 255]
            blurredImage[y][x].red = std::clamp(static_cast<int>(totalRed / count), 0, 255);
            blurredImage[y][x].green = std::clamp(static_cast<int>(totalGreen / count), 0, 255);
            blurredImage[y][x].blue = std::clamp(static_cast<int>(totalBlue / count), 0, 255);
        }
    }

    return blurredImage; // Return the blurred image
}

// Apply motion blur to the image based on a given motion length with one thread
std::vector<std::vector<RGB>> applyMotionBlurSingleThread(const std::vector<std::vector<RGB>>& image, int motionLength) {
    int height = image.size(), width = image[0].size(); // Get the dimensions of the input image
    std::vector<std::vector<RGB>> blurredImage(height, std::vector<RGB>(width)); // Prepare the output image with the same dimensions
    int halfLength = motionLength / 2; // Compute half the motion length to average pixels around the target pixel

    // Iterate through each pixel in the image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double totalRed = 0, totalGreen = 0, totalBlue = 0; // Initialize accumulators for each color channel
            int count = 0; // Initialize a counter to keep track of the number of pixels averaged

            // Iterate over a range of pixels in the motion direction (horizontal)
            for (int mx = -halfLength; mx <= halfLength; ++mx) {
                int currentX = x + mx; // Calculate the x-coordinate of the pixel to be considered
                // Ensure the current pixel is within image bounds
                if (currentX >= 0 && currentX < width) {
                    const RGB& pixel = image[y][currentX]; // Access the pixel at the calculated position
                    // Accumulate the color values of the pixel
                    totalRed += pixel.red;
                    totalGreen += pixel.green;
                    totalBlue += pixel.blue;
                    count++; // Increment the count of pixels considered
                }
            }

            // Calculate the average color value for the target pixel and clamp the values to [0, 255]
            blurredImage[y][x].red = std::clamp(static_cast<int>(totalRed / count), 0, 255);
            blurredImage[y][x].green = std::clamp(static_cast<int>(totalGreen / count), 0, 255);
            blurredImage[y][x].blue = std::clamp(static_cast<int>(totalBlue / count), 0, 255);
        }
    }

    return blurredImage; // Return the image with applied motion blur
}

// Function to calculate Euclidean distance between two colors in RGB space with one thread
double colorDistanceSingleThread(const RGB& color1, const RGB& color2) {
    return std::sqrt(
        (color1.red - color2.red) * (color1.red - color2.red) +
        (color1.green - color2.green) * (color1.green - color2.green) +
        (color1.blue - color2.blue) * (color1.blue - color2.blue)
    );
}

// Apply bucket fill to the other image using one thread with one thread
std::vector<std::vector<RGB>> applyBucketFillSingleThread(const std::vector<std::vector<RGB>>& image, int threshold) {
    int height = image.size(), width = image[0].size();
    const RGB fillColor  = {0, 255, 0}; // Green color
    int seedX = bucketFillX;
    int seedY = bucketFillY;

    std::vector<std::vector<RGB>> bucketFilledImage = image;
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));

    // Check if seed point is within the image
    if (seedX < 0 || seedX >= width || seedY < 0 || seedY >= height) {
        std::cerr << "Seed point is outside the image bounds." << std::endl << std::endl;
        return bucketFilledImage; // Return the original image if seed point is invalid
    }

    RGB targetColor = image[seedY][seedX];

    // Stack for iterative DFS
    std::stack<std::pair<int, int>> stack;
    stack.push({seedX, seedY});

    while (!stack.empty()) {
        int x, y;
        std::tie(x, y) = stack.top();
        stack.pop();

        // Check bounds and if the pixel has already been visited
        if (x < 0 || x >= width || y < 0 || y >= height || visited[y][x]) continue;

        // Improved check using Euclidean distance for color threshold
        if (colorDistanceSingleThread(image[y][x], targetColor) <= threshold) {
            bucketFilledImage[y][x] = fillColor; // Apply fill color
            visited[y][x] = true; // Mark as visited

            // Push neighboring pixels to stack
            stack.push({x + 1, y});
            stack.push({x - 1, y});
            stack.push({x, y + 1});
            stack.push({x, y - 1});
        }
    }

    return bucketFilledImage;
}

// Bicubic interpolation kernel based on Catmull-Rom spline with one thread
double cubicInterpolateSingleThread(double p[4], double x) {
    // Performs the cubic interpolation formula on a set of four points (p[0] to p[3]) and a parameter x
    return p[1] + 0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}

// Function to perform bicubic interpolation on a 4x4 patch of an image
double bicubicInterpolateSingleThread(double arr[4][4], double x, double y) {
    double colArr[4];
    // Interpolates the values along the y-axis
    for (int i = 0; i < 4; i++) {
        colArr[i] = cubicInterpolateSingleThread(arr[i], y);
    }
    // Interpolates the result along the x-axis
    return cubicInterpolateSingleThread(colArr, x);
}

// Function to resize an image using bicubic interpolation with one thread
std::vector<std::vector<RGB>> resizeBicubicSingleThread(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight) {
    int imgHeight = image.size();
    int imgWidth = image[0].size();

    std::vector<std::vector<RGB>> resized(newHeight, std::vector<RGB>(newWidth));
    double xRatio = static_cast<double>(imgWidth) / newWidth;
    double yRatio = static_cast<double>(imgHeight) / newHeight;

    // Loop over each pixel in the new image
    for (int i = 0; i < newHeight; ++i) {
        for (int j = 0; j < newWidth; ++j) {
            double x = (j + 0.5) * xRatio - 0.5;
            double y = (i + 0.5) * yRatio - 0.5;

            int xInt = int(x);
            int yInt = int(y);

            double xDiff = x - xInt;
            double yDiff = y - yInt;

            double redVals[4][4], greenVals[4][4], blueVals[4][4];

            // Collect the 4x4 neighborhood of the current pixel from the original image
            for (int m = -1; m <= 2; ++m) {
                for (int n = -1; n <= 2; ++n) {
                    int xN = std::clamp(xInt + n, 0, imgWidth - 1);
                    int yM = std::clamp(yInt + m, 0, imgHeight - 1);

                    const RGB& pixel = image[yM][xN];
                    redVals[m + 1][n + 1] = pixel.red;
                    greenVals[m + 1][n + 1] = pixel.green;
                    blueVals[m + 1][n + 1] = pixel.blue;
                }
            }

            // Interpolate the color values for the new pixel and ensure they are within the valid range
            resized[i][j].red = std::clamp(static_cast<int>(bicubicInterpolateSingleThread(redVals, xDiff, yDiff)), 0, 255);
            resized[i][j].green = std::clamp(static_cast<int>(bicubicInterpolateSingleThread(greenVals, xDiff, yDiff)), 0, 255);
            resized[i][j].blue = std::clamp(static_cast<int>(bicubicInterpolateSingleThread(blueVals, xDiff, yDiff)), 0, 255);
        }
    }

    return resized;
}

// Function to resize an image using bilinear interpolation with one thread
std::vector<std::vector<RGB>> resizeBilinearSingleThread(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight) {
    int imgHeight = image.size();
    int imgWidth = image[0].size();

    std::vector<std::vector<RGB>> resized(newHeight, std::vector<RGB>(newWidth));
    double xRatio = static_cast<double>(imgWidth - 1) / (newWidth - 1);
    double yRatio = static_cast<double>(imgHeight - 1) / (newHeight - 1);

    // Loop over each pixel in the new image
    for (int i = 0; i < newHeight; ++i) {
        for (int j = 0; j < newWidth; ++j) {
            int xL = std::floor(xRatio * j);
            int yL = std::floor(yRatio * i);
            int xH = std::ceil(xRatio * j);
            int yH = std::ceil(yRatio * i);

            double xWeight = (xRatio * j) - xL;
            double yWeight = (yRatio * i) - yL;

            // Interpolate between the four neighboring pixels to find the new pixel value
            RGB a = image[yL][xL];
            RGB b = xH < imgWidth ? image[yL][xH] : a;
            RGB c = yH < imgHeight ? image[yH][xL] : a;
            RGB d = (xH < imgWidth && yH < imgHeight) ? image[yH][xH] : a;

            // Calculate the new pixel's color values and ensure they are within the valid range
            resized[i][j].red = static_cast<uint8_t>(
                a.red * (1 - xWeight) * (1 - yWeight) +
                b.red * xWeight * (1 - yWeight) +
                c.red * (1 - xWeight) * yWeight +
                d.red * xWeight * yWeight);
            resized[i][j].green = static_cast<uint8_t>(
                a.green * (1 - xWeight) * (1 - yWeight) +
                b.green * xWeight * (1 - yWeight) +
                c.green * (1 - xWeight) * yWeight +
                d.green * xWeight * yWeight);
            resized[i][j].blue = static_cast<uint8_t>(
                a.blue * (1 - xWeight) * (1 - yWeight) +
                b.blue * xWeight * (1 - yWeight) +
                c.blue * (1 - xWeight) * yWeight +
                d.blue * xWeight * yWeight);
        }
    }

    return resized;
}

// Apply nearest neighbor resizing to the image with one thread
std::vector<std::vector<RGB>> nearestNeighborResizeSingleThread(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight) {
        int imageWidth = image.size();
        int imageHeight = image[0].size();

        double widthScale = (double)newWidth / imageWidth;
        double heightScale = (double)newHeight / imageHeight;
        
        std::vector<std::vector<RGB>> resizedImage(newWidth, std::vector<RGB>(newHeight));

        for (int x = 0; x < newWidth; x++) {
            for (int y = 0; y < newHeight; y++) {
                int originalX = (int)(x / widthScale);
                int originalY = (int)(y / heightScale);

                resizedImage[x][y] = image[originalX][originalY];
            }
        }

        return resizedImage;
}

// Save the new image data by copying the original file and replacing the header (for resize) and pixel color data  with one thread
void writeBmpSingleThread(const std::string& filename, const std::vector<std::vector<RGB>>& image, bool resize, int resizedWidth, int resizedHeight) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Could not open output file for writing." << std::endl << std::endl;
        return;
    }


    int width = image[0].size();
    int height = image.size();
    int rowPadding = (4 - (width * 3) % 4) % 4;

    if (resize) {

        int fileSize = 54 + (width*3 + rowPadding) * height; // Header size (54 bytes) + pixel data

        // Simple BMP header for a 24-bit BMP
        unsigned char header[54] = {
            'B','M',  // Signature
            0,0,0,0,  // Image file size in bytes
            0,0,0,0,  // Reserved
            54,0,0,0, // Start of pixel array
            40,0,0,0, // Info header size
            0,0,0,0,  // Image width
            0,0,0,0,  // Image height
            1,0,      // Number of color planes
            24,0,     // Bits per pixel
            0,0,0,0,  // Compression
            0,0,0,0,  // Image size
            0,0,0,0,  // Horizontal resolution
            0,0,0,0,  // Vertical resolution
            0,0,0,0,  // Colors in color table
            0,0,0,0,  // Important color count
        };

        // Fill in the file size, width, and height in the header
        *reinterpret_cast<int*>(&header[2]) = fileSize;
        *reinterpret_cast<int*>(&header[18]) = width;
        *reinterpret_cast<int*>(&header[22]) = height;

        // Write the header
        outFile.write(reinterpret_cast<const char*>(header), 54);

    }

    // Write the pixel data
    for (int y = height - 1; y >= 0; --y) {
        outFile.write(reinterpret_cast<const char*>(image[y].data()), width * sizeof(RGB));
        for (int i = 0; i < rowPadding; ++i) {
            outFile.put(0);
        }
    }
}

struct ThreadData {
    int startRow, endRow;
    std::string filename;
    std::vector<std::vector<RGB>>* image;
    int width, rowPadding;
};

void* readRows(void* arg) {
    ThreadData* data = reinterpret_cast<ThreadData*>(arg);
    std::ifstream bmpFile(data->filename, std::ios::binary);
    if (!bmpFile) {
        std::cerr << "Could not open BMP file!" << std::endl;
        return nullptr;
    }
    
    // Each thread seeks to its starting position before reading
    int dataSize = data->width * sizeof(RGB) + data->rowPadding;
    for (int y = data->startRow; y < data->endRow; y++) {
        bmpFile.seekg(54 + y * dataSize, bmpFile.beg);
        bmpFile.read(reinterpret_cast<char*>((*data->image)[y].data()), data->width * sizeof(RGB));
        bmpFile.ignore(data->rowPadding); // Ignore padding
    }
    
    return nullptr;
}

// Read bitmap images with many threads
std::vector<std::vector<RGB>> readBmpMultipleThreads(const std::string& filename) {
    // read header for width,size
    std::ifstream bmpFile(filename, std::ios::binary);
    if (!bmpFile) {
        std::cerr << "Could not open BMP file!" << std::endl;
        return {};
    }

    int32_t width, height;
    bmpFile.seekg(18);
    bmpFile.read(reinterpret_cast<char*>(&width), sizeof(width));
    bmpFile.read(reinterpret_cast<char*>(&height), sizeof(height));
    int rowPadding = (4 - (width * 3) % 4) % 4;

    // create rgb vector
    std::vector<std::vector<RGB>> image(height, std::vector<RGB>(width));

    // Determine the number of threads to use
    unsigned numThreads = std::thread::hardware_concurrency();
    std::vector<pthread_t> threads(numThreads);
    std::vector<ThreadData> threadData(numThreads);

    // start thread queue
    // each thread reads a row and writes to rgb vector
    int rowsPerThread = height / numThreads;
    for (unsigned i = 0; i < numThreads; ++i) {
        threadData[i].startRow = i * rowsPerThread;
        threadData[i].endRow = (i == numThreads - 1) ? height : (i + 1) * rowsPerThread;
        threadData[i].filename = filename;
        threadData[i].image = &image;
        threadData[i].width = width;
        threadData[i].rowPadding = rowPadding;

        pthread_create(&threads[i], nullptr, readRows, &threadData[i]);
    }

    for (unsigned i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return image;
}

// Generate the Gaussian kernel with many threads
std::vector<std::vector<double>> generateGaussianKernelMultipleThreads(double sigma) {

}

// Apply Gaussian blur to an image with many threads
std::vector<std::vector<RGB>> applyGaussianBlurMultipleThreads(const std::vector<std::vector<RGB>>& image, const std::vector<std::vector<double>>& kernel) {

}

// Apply box blur to the image with many threads
std::vector<std::vector<RGB>> applyBoxBlurMultipleThreads(const std::vector<std::vector<RGB>>& image, int boxSize) {

}

// Apply motion blur to the image based on a given motion length with many threads
std::vector<std::vector<RGB>> applyMotionBlurMultipleThreads(const std::vector<std::vector<RGB>>& image, int motionLength) {

}

// Function to calculate Euclidean distance between two colors in RGB space with many threads
double colorDistanceMultipleThreads(const RGB& color1, const RGB& color2) {

}

// Apply bucket fill to the other image with many threads
std::vector<std::vector<RGB>> applyBucketFillMultipleThreads(const std::vector<std::vector<RGB>>& image, int threshold) {

}

// Bicubic interpolation kernel based on Catmull-Rom spline with many threads
double cubicInterpolateMultipleThreads(double p[4], double x) {

}

// Function to perform bicubic interpolation on a 4x4 patch of an image with many threads
double bicubicInterpolateMultipleThreads(double arr[4][4], double x, double y) {

}

// Function to resize an image using bicubic interpolation with many threads
std::vector<std::vector<RGB>> resizeBicubicMultipleThreads(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight) {

}

// Function to resize an image using bilinear interpolation with many threads
std::vector<std::vector<RGB>> resizeBilinearMultipleThreads(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight) {

}

// Apply nearest neighbor resizing to the image  with many threads
std::vector<std::vector<RGB>> nearestNeighborResizeMultipleThreads(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight) {

}

// Save the new image data by copying the original file and replacing the header (for resize) and pixel color data  with many threads
void writeBmpMultipleThreads(const std::string& filename, const std::vector<std::vector<RGB>>& image, bool resize, int resizedWidth, int resizedHeight) {

}