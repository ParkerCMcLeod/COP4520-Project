import tkinter as tk
from tkinter import ttk
from PIL import Image
from subprocess import Popen, PIPE

# Global variables
root = tk.Tk()
frame = tk.Frame(root, bg='#f0f0f0', bd=10)
status_label = tk.Label(root, text="Ready", font=('Helvetica', 12), bg='#e6e6e6')
input_image_size = tk.StringVar(value='small') 
input_image_path = "in/smallImage.bmp"
input_photo = None
input_label = output_label = None 

def bmp_to_png(bmp_path, max_size=(400, 300)):
    png_path = bmp_path.replace('.bmp', '.png')
    with Image.open(bmp_path) as img:
        img.thumbnail(max_size, Image.LANCZOS)
        img.save(png_path, 'PNG')
    return png_path

def update_status(message):
    status_label.config(text=message)

def update_output_image(function_name):
    output_image_path = f"out/{function_name}.bmp"
    output_image_png_path = bmp_to_png(output_image_path)
    try:
        output_photo = tk.PhotoImage(file=output_image_png_path)
        output_label.configure(image=output_photo)
        output_label.image = output_photo
    except tk.TclError as e:
        print(f"Failed to load output image: {e}")

def run_command(arg):
    update_status(f"Applying {arg}...")
    root.update_idletasks()
    command = f"make run sigma={sigma.get()} boxSize={boxSize.get()} motionLength={motionLength.get()} bucketFillThreshold={bucketFillThreshold.get()} bucketFillX={bucketFillX.get()} bucketFillY={bucketFillY.get()} resizeWidthBilinear={resizeWidthBilinear.get()} resizeHeightBilinear={resizeHeightBilinear.get()} resizeWidthBicubic={resizeWidthBicubic.get()} resizeHeightBicubic={resizeHeightBicubic.get()} resizeWidthNearestNeighbor={resizeWidthNearestNeighbor.get()} resizeHeightNearestNeighbor={resizeHeightNearestNeighbor.get()} input_image_size={input_image_size.get()} function={arg}"
    print(command)
    process = Popen(command, shell=True, stdout=PIPE, stderr=PIPE)
    stdout, stderr = process.communicate()

    if process.returncode == 0:
        print(f"Command executed successfully for {arg}")
        print(stdout.decode())
        update_output_image(arg)
        update_status(f"{arg} applied successfully.")
    else:
        print(f"Error in command execution for {arg}")
        print(stderr.decode())
        update_status(f"Error applying {arg}.")

def create_image_size_radio_buttons(frame):
    size_label_frame = tk.Frame(frame, bg='#e6e6e6')
    size_label_frame.pack(fill='x', expand=True, padx=5, pady=5)
    size_label = tk.Label(size_label_frame, text="Input Image Size:", bg='#e6e6e6')
    size_label.pack(side='left')

    sizes = {"Small": "small", "Medium": "medium", "Large": "large"}
    for text, size in sizes.items():
        rb = tk.Radiobutton(size_label_frame, text=text, variable=input_image_size, value=size, bg='#e6e6e6')
        rb.pack(side='left', padx=5)

def setup_ui():
    root.title("Image Processor")
    set_window_position(root, 1280, 720)
    frame.pack(padx=5, pady=5, expand=True)
    status_label.pack(side='bottom', pady=(5, 5), fill='x')
    root.configure(bg='#e6e6e6')
    create_image_size_radio_buttons(frame)
    setup_image_frames()
    create_sliders_and_buttons()

# Centers the window on the screen
def set_window_position(window, width, height):
    screen_width = window.winfo_screenwidth()
    screen_height = window.winfo_screenheight()
    center_x = int((screen_width - width) / 2)
    center_y = int((screen_height - height) / 2) - 25
    window.geometry(f"{width}x{height}+{center_x}+{center_y}")

def setup_image_frames():
    global input_photo, input_label, output_label
    input_image_png_path = bmp_to_png(input_image_path)
    input_photo = tk.PhotoImage(file=input_image_png_path)
    input_label = setup_image_frame("left", input_photo, "Original")
    output_label = setup_image_frame("right", input_photo, "Modified")

def setup_image_frame(side, photo, text):
    label_frame = tk.Frame(frame, bg='#f0f0f0')
    label_frame.pack(side=side, padx=(5, 5), expand=True)
    label = tk.Label(label_frame, image=photo)
    label.pack()
    label.image = photo
    tk.Label(label_frame, text=text, bg='#f0f0f0').pack()
    return label

def create_sliders_and_buttons():
    configurations = [
        {"slider_name": "sigma", "label": "Sigma", "function_name": "gaussianBlur", "color": "#B28DFF", "default": 3.0},  # Light Purple
        {"slider_name": "boxSize", "label": "Box Size", "function_name": "boxBlur", "color": "#FFD580", "default": 9},  # Light Orange
        {"slider_name": "motionLength", "label": "Motion Length", "function_name": "motionBlur", "color": "#80CBC4", "default": 15},  # Teal
        {"slider_name": "bucketFillThreshold", "label": "Bucket Fill Threshold", "function_name": "bucketFill", "color": "#FFABAB", "default": 10},  # Light Red
        {"slider_name": "bucketFillX", "label": "Bucket Fill X", "function_name": "bucketFill", "color": "#FFABAB", "default": 512},  # Light Red
        {"slider_name": "bucketFillY", "label": "Bucket Fill Y", "function_name": "bucketFill", "color": "#FFABAB", "default": 384},  # Light Red
        {"slider_name": "resizeWidthBilinear", "label": "Resize Width", "function_name": "bilinearResize", "color": "#A7FFEB", "default": 100},  # Mint Green
        {"slider_name": "resizeHeightBilinear", "label": "Resize Height", "function_name": "bilinearResize", "color": "#A7FFEB", "default": 100},  # Mint Green
        {"slider_name": "resizeWidthBicubic", "label": "Resize Width", "function_name": "bicubicResize", "color": "#FFC4E1", "default": 100},  # Pink Lavender
        {"slider_name": "resizeHeightBicubic", "label": "Resize Height", "function_name": "bicubicResize", "color": "#FFC4E1", "default": 100},  # Pink Lavender
        {"slider_name": "resizeWidthNearestNeighbor", "label": "Resize Width", "function_name": "nearestNeighborResize", "color": "#B9FBC0", "default": 100},  # Pastel Green
        {"slider_name": "resizeHeightNearestNeighbor", "label": "Resize Height", "function_name": "nearestNeighborResize", "color": "#B9FBC0", "default": 100},  # Pastel Green
    ]
    for config in configurations:
        create_slider_and_button(frame, **config)

def create_slider_and_button(frame, **kwargs):
    slider_name = kwargs['slider_name']
    label_text = kwargs['label']
    function_name = kwargs['function_name']
    default = kwargs['default']
    global sigma, boxSize, motionLength, bucketFillThreshold, bucketFillX, bucketFillY, resizeWidthBilinear, resizeHeightBilinear, resizeWidthBicubic, resizeHeightBicubic, resizeWidthNearestNeighbor, resizeHeightNearestNeighbor
    
    container_frame = tk.Frame(frame, bg='#e6e6e6')
    container_frame.pack(fill='x', expand=True, padx=5, pady=5)

    slider_label = tk.Label(container_frame, text=label_text, bg='#e6e6e6')
    slider_label.pack(side='left')

    button = ttk.Button(container_frame, text=f"Apply {function_name}", command=lambda: run_command(function_name))
    button.pack(side='left', padx=(5, 5))

    slider = tk.Scale(container_frame, from_=0, to=default*3, orient=tk.HORIZONTAL, length=200)
    slider.set(default)
    slider.pack(side='right', fill='x', expand=True)

    globals()[slider_name] = slider

setup_ui()
root.mainloop()