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

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

std::string InputFilename; // Input
const std::string GaussianBlurredOutputFilename = "out/gaussianBlur.bmp"; // Output
const std::string BoxBlurredOutputFilename = "out/boxBlur.bmp"; // Output
const std::string MotionBlurredOutputFilename = "out/motionBlur.bmp"; // Output
const std::string BucketFillOutputFilename = "out/bucketFill.bmp"; // Output
const std::string BilinearResizedOutputFilename = "out/bilinearResize.bmp"; // Output
const std::string BicubicResizedOutputFilename = "out/bicubicResize.bmp"; // Output
const std::string NearestNeighborResizedOutputFilename = "out/nearestNeighborResize.bmp"; // Output

double sigma = 3.0; // Gaussian blur sigma value (blur radius, significant performance impact)
int boxSize = 9; // Box blur value (blur radius, must be odd)
int motionLength = 15; // Define the length of the motion blur
int bucketFillThreshold = 10; // Threshold for bucket fill
int bucketFillX = 504; // X pixel location for starting bucket fill
int bucketFillY = 341; // Y pixel location for starting bucket fill
int resizeWidth = 800; // Desired resize width
int resizeHeight = 600; // Desired resize height
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
// Read and processes bitmap images
std::vector<std::vector<RGB>> readBmp(const std::string& filename);
// Write and resize images
void writeBmpResize(const std::string& filename, const std::vector<std::vector<RGB>>& image, int resizedWidth, int resizedHeight);
// Generate the Gaussian kernel to support Gaussian blur
std::vector<std::vector<double>> generateGaussianKernel(double sigma);
// Apply Gaussian blur to the image
std::vector<std::vector<RGB>> applyGaussianBlur(const std::vector<std::vector<RGB>>& image, const std::vector<std::vector<double>>& kernel);
// Save the new image data by copying the original file and replacing the pixel color data
void writeBmp(const std::string& filename, const std::vector<std::vector<RGB>>& image);
// Apply box blur to the image
std::vector<std::vector<RGB>> applyBoxBlur(const std::vector<std::vector<RGB>>& image, int boxSize);
// Apply motion blur to the image
std::vector<std::vector<RGB>> applyMotionBlur(const std::vector<std::vector<RGB>>& image, int motionLength);
// Apply bucket fill to the other image
std::vector<std::vector<RGB>> applyBucketFill(const std::vector<std::vector<RGB>>& image, int threshold);
// Apply bilinear resizing to the image
std::vector<std::vector<RGB>> resizeBilinear(const std::vector<std::vector<RGB>>& image, int outputWidth, int outputHeight);
// Apply bicubic resizing to the image
std::vector<std::vector<RGB>> resizeBicubic(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight);
// Apply nearest neighbor resizing to the image
std::vector<std::vector<RGB>> nearestNeighborResize(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight);

int main(int argc, char* argv[]) {
    if (argc < 11) {
        std::cerr << "Usage: " << argv[0] << " <sigma> <boxSize> <motionLength> <bucketFillThreshold> <bucketFillX> <bucketFillY> <resizeWidth> <resizeHeight> <inputImageSize> <function>" << std::endl;
        return 1;
    }

    // Parse the command-line arguments
    sigma = std::atof(argv[1]);
    boxSize = std::atoi(argv[2]);
    motionLength = std::atoi(argv[3]);
    bucketFillThreshold = std::atoi(argv[4]);
    bucketFillX = std::atoi(argv[5]);
    bucketFillY = std::atoi(argv[6]);
    resizeWidth = std::atoi(argv[7]);
    resizeHeight = std::atoi(argv[8]);
    inputImageSize = argv[9];
    function = argv[10];

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

// Helper function for parsing image
std::vector<std::vector<RGB>> parseImageHelper() {
    auto start = std::chrono::high_resolution_clock::now(); // Start timing
    std::cout << std::endl << "Parsing input image..." << std::endl;
    auto image = readBmp(InputFilename); // Read and process the image from file
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    std::chrono::duration<double> elapsed = end - start; // Calculate elapsed time
    std::cout << "Time taken for parsing input image (" << (image[0].size() * image.size()) << "px): " << elapsed.count() << " seconds." << std::endl << std::endl;

    return image;
}

// Helper function for timing and implementing the gaussian blur function
void gaussianBlurHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying Gaussian blur (sigma=" << sigma << ")..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); // Reset start time
    auto kernel = generateGaussianKernel(sigma); // Generate the Gaussian kernel (precompute the blur matrix values)
    auto blurredImage = applyGaussianBlur(image, kernel); // Apply Gaussian blur to the image
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    auto elapsed = end - start; // Calculate elapsed time
    std::cout << "Time taken for applying Gaussian blur: " << elapsed.count() << " seconds." << std::endl;
    writeBmp(GaussianBlurredOutputFilename, blurredImage); // Write the blurred image to a new file
    std::cout << "Saved gaussian blurred image to \"" << GaussianBlurredOutputFilename << "\"" << std::endl << std::endl;
}

// Helper function for timing and implementing the box blur function
void boxBlurHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying box blur (boxSize=" << boxSize << ")..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); // Reset start time
    auto boxBlurredImage = applyBoxBlur(image, boxSize);
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    auto elapsed = end - start; // Calculate elapsed time
    std::cout << "Time taken for applying box blur: " << elapsed.count() << " seconds." << std::endl;
    writeBmp(BoxBlurredOutputFilename, boxBlurredImage); // Write the blurred image to a new file
    std::cout << "Saved box-blurred image to \"" << BoxBlurredOutputFilename << "\"" << std::endl << std::endl;
}

// Helper function for timing and implementing the motion blur function
void motionBlurHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying motion blur (motionLength=" << motionLength << ")..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); // Reset start time
    auto motionBlurredImage = applyMotionBlur(image, motionLength);
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    auto elapsed = end - start; // Calculate elapsed time
    std::cout << "Time taken for applying motion blur: " << elapsed.count() << " seconds." << std::endl;
    writeBmp(MotionBlurredOutputFilename, motionBlurredImage); // Write the motion-blurred image to a new file
    std::cout << "Saved motion-blurred image to \"" << MotionBlurredOutputFilename << "\"" << std::endl << std::endl;
}

// Helper function for timing and implementing the bucket fill function
void bucketFillHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying bucket fill (Threshold=" << bucketFillThreshold << ")..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); // Reset start time
    auto bucketFilledImage = applyBucketFill(image, bucketFillThreshold);
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    auto elapsed = end - start; // Calculate elapsed time
    std::cout << "Time taken for applying bucket fill: " << elapsed.count() << " seconds." << std::endl;
    writeBmp(BucketFillOutputFilename, bucketFilledImage); // Write the bucket-filled image to a new file
    std::cout << "Saved bucket-filled image to \"" << BucketFillOutputFilename << "\"" << std::endl << std::endl;
}

// Helper function for timing and implementing the bilinear resize function
void bilinearResizeHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying bilinear resizing (Output Size=" << resizeWidth << "x" << resizeHeight << ")..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); // Reset start time
    auto bilinearResizedImage = resizeBilinear(image, resizeWidth, resizeHeight);
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    auto elapsed = end - start; // Calculate elapsed time
    std::cout << "Time taken for applying bilinear resizing: " << elapsed.count() << " seconds." << std::endl;
    writeBmpResize(BilinearResizedOutputFilename, bilinearResizedImage, resizeWidth, resizeHeight); // Write the resized image to a new file
    std::cout << "Saved bilinear-resized image to \"" << BilinearResizedOutputFilename << "\"" << std::endl << std::endl;
}

// Helper function for timing and implementing the bicubic resize function
void bicubicResizeHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying bicubic resizing (Output Size=" << resizeWidth << "x" << resizeHeight << ")..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); // Reset start time
    auto bicubicResizedImage = resizeBicubic(image, resizeWidth, resizeHeight); // Use bicubic resizing
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    auto elapsed = end - start; // Calculate elapsed time
    std::cout << "Time taken for applying bicubic resizing: " << elapsed.count() << " seconds." << std::endl;
    writeBmpResize(BicubicResizedOutputFilename, bicubicResizedImage, resizeWidth, resizeHeight); // Write the resized image to a new file
    std::cout << "Saved bicubic-resized image to \"" << BicubicResizedOutputFilename << "\"" << std::endl << std::endl;
}

// Helper function for timing and implementing the nearest neighbor resize function
void nearestNeighborResizeHelper(std::vector<std::vector<RGB>> image) {
    std::cout << "Applying nearest neighbor resizing (Output Size=" << resizeWidth << "x" << resizeHeight << ")..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); // Reset start time
    auto nearestNeighborResizedImage = nearestNeighborResize(image, resizeWidth, resizeHeight); // Use nearest neighbor resizing
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    auto elapsed = end - start; // Calculate elapsed time
    std::cout << "Time taken for applying nearest neighbor resizing: " << elapsed.count() << " seconds." << std::endl;
    writeBmpResize(NearestNeighborResizedOutputFilename, nearestNeighborResizedImage, resizeWidth, resizeHeight); // Write the resized image to a new file
    std::cout << "Saved nearestNeighbor-resized image to \"" << NearestNeighborResizedOutputFilename << "\"" << std::endl << std::endl;
}

// Read bitmap images
std::vector<std::vector<RGB>> readBmp(const std::string& filename) {
    std::ifstream bmpFile(filename, std::ios::binary); // Open the BMP file in binary mode
    std::vector<std::vector<RGB>> image; // Create a 2D vector to store the pixels
    if (!bmpFile) {
        std::cerr << "Could not open BMP file!" << std::endl; // Check if the file was successfully opened
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

// Generate the Gaussian kernel
std::vector<std::vector<double>> generateGaussianKernel(double sigma) {
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

// Apply Gaussian blur to an image
std::vector<std::vector<RGB>> applyGaussianBlur(const std::vector<std::vector<RGB>>& image, const std::vector<std::vector<double>>& kernel) {
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

// Save the new image data by copying the original file and replacing the pixel color data
void writeBmp(const std::string& filename, const std::vector<std::vector<RGB>>& image) {
    std::ifstream bmpFile(InputFilename, std::ios::binary); // Open the original BMP file in binary mode to read the header
    std::ofstream outFile(filename, std::ios::binary); // Open the output file in binary mode

    if (!bmpFile || !outFile) {
        std::cerr << "Could not open BMP files for reading/writing." << std::endl;
        return;
    }

    // Copy the header from the original BMP file
    std::vector<char> header(54);
    bmpFile.read(header.data(), 54);
    outFile.write(header.data(), 54);

    int width = image[0].size(), height = image.size();
    int rowPadding = (4 - (width * 3) % 4) % 4;

    // Write the new image data
    for (int y = height - 1; y >= 0; y--) {
        outFile.write(reinterpret_cast<const char*>(image[y].data()), width * sizeof(RGB));
        for (int i = 0; i < rowPadding; i++) {
            outFile.put(0);
        }
    }
}

void writeBmpResize(const std::string& filename, const std::vector<std::vector<RGB>>& image, int resizedWidth, int resizedHeight) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Could not open output file for writing." << std::endl;
        return;
    }

    int width = image[0].size();
    int height = image.size();
    int rowPadding = (4 - (width * 3) % 4) % 4;
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

    // Write the pixel data
    for (int y = height - 1; y >= 0; --y) {
        outFile.write(reinterpret_cast<const char*>(image[y].data()), width * sizeof(RGB));
        for (int i = 0; i < rowPadding; ++i) {
            outFile.put(0);
        }
    }
}

// Apply box blur to the image
std::vector<std::vector<RGB>> applyBoxBlur(const std::vector<std::vector<RGB>>& image, int boxSize) {
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

// Apply motion blur to the image based on a given motion length
std::vector<std::vector<RGB>> applyMotionBlur(const std::vector<std::vector<RGB>>& image, int motionLength) {
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

// Function to calculate Euclidean distance between two colors in RGB space
double colorDistance(const RGB& color1, const RGB& color2) {
    return std::sqrt(
        (color1.red - color2.red) * (color1.red - color2.red) +
        (color1.green - color2.green) * (color1.green - color2.green) +
        (color1.blue - color2.blue) * (color1.blue - color2.blue)
    );
}
std::vector<std::vector<RGB>> applyBucketFill(const std::vector<std::vector<RGB>>& image, int threshold) {
    int height = image.size(), width = image[0].size();
    const RGB fillColor  = {0, 255, 0}; // Green color
    int seedX = bucketFillX;
    int seedY = bucketFillY;

    std::vector<std::vector<RGB>> bucketFilledImage = image;
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));

    // Check if seed point is within the image
    if (seedX < 0 || seedX >= width || seedY < 0 || seedY >= height) {
        std::cerr << "Seed point is outside the image bounds." << std::endl;
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
        if (colorDistance(image[y][x], targetColor) <= threshold) {
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

// Bicubic interpolation kernel based on Catmull-Rom spline
double cubicInterpolate(double p[4], double x) {
    // Performs the cubic interpolation formula on a set of four points (p[0] to p[3]) and a parameter x
    return p[1] + 0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}

// Function to perform bicubic interpolation on a 4x4 patch of an image
double bicubicInterpolate(double arr[4][4], double x, double y) {
    double colArr[4];
    // Interpolates the values along the y-axis
    for (int i = 0; i < 4; i++) {
        colArr[i] = cubicInterpolate(arr[i], y);
    }
    // Interpolates the result along the x-axis
    return cubicInterpolate(colArr, x);
}

// Function to resize an image using bicubic interpolation
std::vector<std::vector<RGB>> resizeBicubic(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight) {
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
            resized[i][j].red = std::clamp(static_cast<int>(bicubicInterpolate(redVals, xDiff, yDiff)), 0, 255);
            resized[i][j].green = std::clamp(static_cast<int>(bicubicInterpolate(greenVals, xDiff, yDiff)), 0, 255);
            resized[i][j].blue = std::clamp(static_cast<int>(bicubicInterpolate(blueVals, xDiff, yDiff)), 0, 255);
        }
    }

    return resized;
}

// Function to resize an image using bilinear interpolation
std::vector<std::vector<RGB>> resizeBilinear(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight) {
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

std::vector<std::vector<RGB>> nearestNeighborResize(const std::vector<std::vector<RGB>>& image, int newWidth, int newHeight) {
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