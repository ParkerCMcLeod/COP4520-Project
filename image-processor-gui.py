from PIL import Image
import tkinter as tk
from tkinter import ttk
from subprocess import Popen, PIPE

def bmp_to_png(bmp_path, max_size=(400, 300)):
    """
    Converts a BMP image to PNG, resizes it to fit within max_size, and returns the path of the PNG image.
    """
    png_path = bmp_path.replace('.bmp', '.png')
    with Image.open(bmp_path) as img:
        img.thumbnail(max_size, Image.LANCZOS)
        img.save(png_path, 'PNG')
    return png_path

def run_command(arg):
    """
    Executes a shell command with the specified argument and updates the output image display.
    """
    update_status(f"Executing {arg}...")
    root.update_idletasks()

    command = f"make run ARGS={arg}"
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

def update_output_image(function_name):
    output_image_path = f"out/{function_name}.bmp"
    output_image_png_path = bmp_to_png(output_image_path)
    try:
        output_photo = tk.PhotoImage(file=output_image_png_path)
        output_label.configure(image=output_photo)
        output_label.image = output_photo
    except tk.TclError as e:
        print(f"Failed to load output image: {e}")

def update_status(message):
    status_label.config(text=message)

def setup_ui():
    root.title("Image Processor")
    set_window_position(root, 800, 600)
    style = ttk.Style(root)
    style.theme_use('clam')
    create_image_frames()
    create_buttons(functions)
    status_label.pack(side='bottom', pady=(10, 50), fill='x')
    root.configure(bg='#e6e6e6')
    frame.pack(padx=30, pady=30, expand=True)

def set_window_position(window, width, height):
    screen_width = window.winfo_screenwidth()
    screen_height = window.winfo_screenheight()
    center_x = int((screen_width - width) / 2)
    center_y = int((screen_height - height) / 2)
    window.geometry(f"{width}x{height}+{center_x}+{center_y}")

def create_image_frames():
    global input_label, output_label
    frame.place(relwidth=0.8, relheight=0.7, relx=0.5, rely=0.4, anchor='center')
    setup_image_frame("left", input_photo, "Original")
    output_label = setup_image_frame("right", input_photo, "Modified")

def setup_image_frame(side, photo, text):
    label_frame = tk.Frame(frame)
    label_frame.pack(side=side, padx=(20,0))
    label = tk.Label(label_frame, image=photo)
    label.pack()
    label.image = photo  # Keep a reference to avoid garbage collection
    tk.Label(label_frame, text=text, bg='#f0f0f0').pack()
    return label

def create_buttons(function_names):
    for func in function_names:
        ttk.Button(frame, text=func, command=lambda f=func: run_command(f)).pack(pady=5, fill='x', padx=10)

# Global variables
root = tk.Tk()
frame = tk.Frame(root, bg='#f0f0f0', bd=10)
functions = ["gaussianBlur", "boxBlur", "motionBlur", "bucketFill", "bilinearResize", "bicubicResize", "nearestNeighborResize"]
input_image_path = "in/smallImage.bmp"
input_image_png_path = bmp_to_png(input_image_path)
input_photo = tk.PhotoImage(file=input_image_png_path)
status_label = tk.Label(root, text="Ready", font=('Helvetica', 12), bg='#e6e6e6')

setup_ui()
root.mainloop()