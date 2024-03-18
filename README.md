# COP4520-Project
This project focuses on enhancing image processing speeds by focusing on the transition from
single-threaded to multithreaded algorithm implementations. In these implementations, we will
be employing a range of multithreading techniques taught in this course. Concentrating on three
fundamental image processing algorithms, we aim to substantially decrease processing times
and boost computational efficiency. These changes will allow the algorithms to fully utilize
today’s multi-core architectures

https://en.wikipedia.org/wiki/BMP_file_format
https://en.wikipedia.org/wiki/Gaussian_blur

Assumptions: 24-bit, uncompressed, BITMAPINFOHEADER header type, no color space info, bottom up bitmap .bmp file input (project focus is on multithreading image processing, not image parsing)

**Usage**

- To compile the project and create the executable, simply run:
  ```
  make
  ```
  
- To run the compiled project (which also compiles it if necessary), use:
  ```
  make run
  ```
  
- To clean the project directory by removing the executable and object files, execute:
  ```
  make clean
  ```
