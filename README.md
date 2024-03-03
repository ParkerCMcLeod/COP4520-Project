# COP4520-Project
This project focuses on enhancing image processing speeds by focusing on the transition from
single-threaded to multithreaded algorithm implementations. In these implementations, we will
be employing a range of multithreading techniques taught in this course. Concentrating on three
fundamental image processing algorithms, we aim to substantially decrease processing times
and boost computational efficiency. These changes will allow the algorithms to fully utilize
today’s multi-core architectures

https://en.wikipedia.org/wiki/BMP_file_format

Assumptions: 24-bit, uncompressed, BITMAPINFOHEADER header type, no color space info, bottom up bitmap .bmp file input (project focus is on multithreading image processing, not image parsing)

g++ -Wall -std=c++14 image-processor.cpp -o image-processor && ./image-processor 