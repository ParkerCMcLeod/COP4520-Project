#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

// structure to hold rgb values of a pixel
struct RGB {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

// function to read a bmp file and return a 2d array of rgb values
std::vector<std::vector<RGB>> readBMP(const std::string& filename) {
    std::ifstream bmpFile(filename, std::ios::binary);
    std::vector<std::vector<RGB>> rgbData;

    if (!bmpFile) {
        std::cerr << "Could not open BMP file!" << std::endl;
        return rgbData; // return an empty vector if file could not be opened
    }

    // read the bmp header
    bmpFile.seekg(18); // skip the first 18 bytes of the file header to get to the width and height
    int32_t width; // 4 bytes size
    int32_t height; // 4 bytes size
    bmpFile.read(reinterpret_cast<char*>(&width), sizeof(width)); 
    bmpFile.read(reinterpret_cast<char*>(&height), sizeof(height));

    // allocate the 2d array
    rgbData.resize(height, std::vector<RGB>(width));

    // bmp rows are padded to ensure they are a multiple of 4 bytes
    int rowPadding = (4 - (width * 3) % 4) % 4;

    // read the pixel data
    bmpFile.seekg(54); // skip to the pixel data
    for (int y = height - 1; y >= 0; y--) { // bmp stores pixels bottom-to-top
        for (int x = 0; x < width; x++) {
            RGB pixel;
            bmpFile.read(reinterpret_cast<char*>(&pixel), sizeof(RGB));
            rgbData[y][x] = pixel;
        }
        bmpFile.ignore(rowPadding); // skip row padding
    }

    bmpFile.close();
    return rgbData;
}

int main() {
    std::string filename = "small-image.bmp";
    // std::string filename = "medium-image.bmp";
    // std::string filename = "large-image.bmp";
    auto rgbData = readBMP(filename);

    // just for demonstration: print the rgb values of the first pixel
    if (!rgbData.empty()) {
        std::cout << "RGB of the first pixel: ";
        std::cout << "R=" << static_cast<int>(rgbData[0][0].red) << ", ";
        std::cout << "G=" << static_cast<int>(rgbData[0][0].green) << ", ";
        std::cout << "B=" << static_cast<int>(rgbData[0][0].blue) << std::endl;
    }

    return 0;
}