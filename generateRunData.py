import csv
import os
import re
import subprocess
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from numpy.polynomial.polynomial import Polynomial


GENERATE_RUNS = True
PARSE_RUN_OUTPUTS = True
GRAPH_OUTPUTS = True

if (GENERATE_RUNS):
    functions = ['gaussianBlur', 'boxBlur', 'motionBlur', 'bucketFill', 'bilinearResize', 'bicubicResize', 'nearestNeighborResize']
    imageSizes = ['small', 'medium', 'large']

    # Ensure the output directory exists
    outputDir = "./runs"
    if not os.path.exists(outputDir):
        os.makedirs(outputDir)

    # Loop through each combination of function and imageSize
    for function in functions:
        for imageSize in imageSizes:
            cmd = f"make run sigma=3.0 boxSize=9 motionLength=15 bucketFillThreshold=75 bucketFillX=800 bucketFillY=170 resizeWidthBilinear=500 resizeHeightBilinear=745 resizeWidthBicubic=500 resizeHeightBicubic=745 resizeWidthNearestNeighbor=500 resizeHeightNearestNeighbor=745 inputImageSize={imageSize} function={function}"

            outputFile = f"{outputDir}/{imageSize}_{function}.txt"

            with open(outputFile, 'w') as file:
                subprocess.run(cmd, stdout=file, shell=True, check=True)

    print("Commands executed and outputs saved.")


if (PARSE_RUN_OUTPUTS):

    directory = "runs"
    csv_file = "runData.csv"
    headers = [
        "inputFileSize", "function", "pixelCount", 
        "parsingTimeSingleThread", "parsingTimeMultipleThreads", 
        "parsingSpeedupFactor", "timeTakenFunctionExecutionSingleThread", 
        "timeTakenFunctionExecutionMultipleThreads", "functionExecutionSpeedupFactor"
    ]

    if os.path.exists(csv_file):
        os.remove(csv_file)

    with open(csv_file, 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(headers)

    def parse_text(file_path):
        with open(file_path, 'r') as file:
            content = file.read()

            file_size = re.search(r'./image-processor.exe.* (\w+) ', content)
            function = re.search(r'./image-processor.exe.* (\w+)', content)
            pixel_count = re.search(r'Time taken for parsing input image using (?:a single|multiple) threads.*\((\d+)px\)', content)
            parsing_time_single = re.search(r'Time taken for parsing input image using a single thread.*: (\d+)', content)
            parsing_time_multi = re.search(r'Time taken for parsing input image using multiple threads.*: (\d+)', content)
            parsing_speedup = re.search(r'Multithreading speedup factor: (\d+\.\d+)x', content, re.MULTILINE)
            execution_time_single = re.search(r'Time taken for applying .* using a single thread: (\d+)', content)
            execution_time_multi = re.search(r'Time taken for applying .* using multiple threads: (\d+)', content)
            execution_speedup = re.findall(r'Multithreading speedup factor: (\d+\.\d+)x', content)

            # Account for multiple instances of speedup factors if present
            execution_speedup_factor = execution_speedup[1] if len(execution_speedup) > 1 else execution_speedup[0] if execution_speedup else "N/A"

            return [
                file_size.group(1) if file_size else "N/A",
                function.group(1) if function else "N/A",
                pixel_count.group(1) if pixel_count else "N/A",
                parsing_time_single.group(1) if parsing_time_single else "N/A",
                parsing_time_multi.group(1) if parsing_time_multi else "N/A",
                parsing_speedup.group(1) if parsing_speedup else "N/A",
                execution_time_single.group(1) if execution_time_single else "N/A",
                execution_time_multi.group(1) if execution_time_multi else "N/A",
                execution_speedup_factor
            ]

    for filename in os.listdir(directory):
        if filename.endswith(".txt"):
            file_path = os.path.join(directory, filename)
            data = parse_text(file_path)
            
            with open(csv_file, 'a', newline='') as file:
                writer = csv.writer(file)
                writer.writerow(data)

    print("Data extraction and CSV update complete.")

if (GRAPH_OUTPUTS):
    # Load the CSV data into a DataFrame
    df = pd.read_csv('runData.csv')

    # List of functions to plot
    functions = df['function'].unique()

    for function in functions:
        # Filter data for the current function
        func_data = df[df['function'] == function]
        
        # Extract pixelCount, singleThread, and multiThread execution times
        x = func_data['pixelCount']
        y_single = func_data['timeTakenFunctionExecutionSingleThread']
        y_multi = func_data['timeTakenFunctionExecutionMultipleThreads']
        
        # Fit a quadratic curve (2nd degree polynomial) for single-threaded execution times
        coefs_single = Polynomial.fit(x, y_single, 2).convert().coef
        # Fit a quadratic curve for multi-threaded execution times
        coefs_multi = Polynomial.fit(x, y_multi, 2).convert().coef
        
        # Generate a range of pixel counts for plotting the fitted curves
        x_range = np.linspace(x.min(), x.max(), 500)
        # Calculate the fitted values using the quadratic equation
        y_single_fit = coefs_single[0] + coefs_single[1]*x_range + coefs_single[2]*x_range**2
        y_multi_fit = coefs_multi[0] + coefs_multi[1]*x_range + coefs_multi[2]*x_range**2
        
        # Plotting
        plt.figure(figsize=(10, 6))
        plt.plot(x_range, y_single_fit, label='Single Threaded', color='blue')
        plt.plot(x_range, y_multi_fit, label='Multi Threaded', color='red')
        plt.scatter(x, y_single, color='blue', alpha=0.5)  # Original data points
        plt.scatter(x, y_multi, color='red', alpha=0.5)  # Original data points
        plt.title(f'{function} performance vs pixel count')
        plt.xlabel('Pixel Count')
        plt.ylabel('Execution Time (ms)')
        plt.legend()
        plt.grid(True)
        plt.show()