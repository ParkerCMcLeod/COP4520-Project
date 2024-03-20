# COP4520-Project: Multithreaded Image Processing

## Project Overview

This project focuses on enhancing image processing speeds by focusing on the transition from
single-threaded to multithreaded algorithm implementations. In these implementations, we will
be employing a range of multithreading techniques taught in this course. Concentrating on three
fundamental image processing algorithms, we aim to substantially decrease processing times
and boost computational efficiency. These changes will allow the algorithms to fully utilize
today’s multi-core architectures

### Key Assumptions

- Input: 24-bit, uncompressed, BITMAPINFOHEADER type BMP files.
- No color space information included.
- Files are bottom-up bitmaps.
- The focus is on multithreading in image processing, rather than image parsing.

### Relevant Resources

For further reading on the image processing concepts and algorithms mentioned in this project, the following Wikipedia articles are recommended:

- [BMP file format](https://en.wikipedia.org/wiki/BMP_file_format)
- [Gaussian blur](https://en.wikipedia.org/wiki/Gaussian_blur)
- [Box blur](https://en.wikipedia.org/wiki/Box_blur)
- [Motion blur](https://en.wikipedia.org/wiki/Motion_blur)
- [Flood fill (related to bucket fill)](https://en.wikipedia.org/wiki/Flood_fill)
- [Image scaling (covers bilinear, bicubic, and nearest neighbor resizing)](https://en.wikipedia.org/wiki/Image_scaling)

## Getting Started

### Prerequisites

- `make` must be installed on your system.
- Your environment should support C++ compilation.

### Installation

To compile the project, install dependencies, and create the executable, run:

```bash
make
```

To clean the project directory by removing executable and object files, execute:

```bash
make clean
```

## Usage Documentation

### Command Structure

Execute the tool with the `make run` command followed by specific parameters. If a parameter is not provided, it will default to its preset value.

```bash
make run [PARAMETERS]
```

### Parameters and Default Values

Detailed list of parameters along with their default values:

- **sigma=<double>**: Sigma value for Gaussian blur (default: `3.0`).
- **boxSize=<int>**: Size of the box for box blur (default: `9`).
- **motionLength=<int>**: Length of the motion for motion blur (default: `15`).
- **bucketFillThreshold=<int>**: Color difference threshold for bucket fill (default: `75`).
- **bucketFillX=<int>**: X-coordinate starting point for bucket fill (default: `800`).
- **bucketFillY=<int>**: Y-coordinate starting point for bucket fill (default: `170`).
- **resizeWidthBilinear=<int>**: Target width for resizing using bilinear interpolation (default: `500`).
- **resizeHeightBilinear=<int>**: Target height for resizing using bilinear interpolation (default: `745`).
- **resizeWidthBicubic=<int>**: Target width for resizing using bicubic interpolation (default: `500`).
- **resizeHeightBicubic=<int>**: Target height for resizing using bicubic interpolation (default: `745`).
- **resizeWidthNearestNeighbor=<int>**: Target width for resizing using nearest neighbor interpolation (default: `500`).
- **resizeHeightNearestNeighbor=<int>**: Target height for resizing using nearest neighbor interpolation (default: `745`).
- **inputImageSize=<std::string>**: Specifies the size of the input image. Options: `small`, `medium`, `large` (default: `small`).
- **function=<std::string>**: The image processing function to be applied. Options include `all`, `gaussianBlur`, `boxBlur`, `motionBlur`, `bucketFill`, `bilinearResize`, `bicubicResize`, `nearestNeighborResize` (default: `all`).

### Example Usage

To apply a box blur with a custom sigma value to a medium-sized image, while utilizing default values for unspecified parameters:

```bash
make run sigma=2.5 function=boxBlur inputImageSize=medium
```

### Additional Notes

- Match the parameters to the specific needs of your image processing task.
- The tool performs parameter validation, alerting you to any discrepancies.