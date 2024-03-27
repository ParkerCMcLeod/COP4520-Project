import tkinter as tk
from tkinter import ttk
from PIL import Image
from subprocess import Popen, PIPE, STDOUT

class ImageProcessorApp:
    def __init__(self, root):
        self.root = root
        self.setup_ui()

    def setup_ui(self):
        self.root.title("Image Processor")
        self.set_window_position(1200, 800)

        controls_frame = tk.Frame(self.root, bg='#f0f0f0', bd=10)
        right_section_frame = tk.Frame(self.root, bg='#f0f0f0')
        self.images_frame = tk.Frame(right_section_frame, bg='#f0f0f0')
        self.console_frame = tk.Frame(right_section_frame, bg='#f0f0f0')
        
        controls_frame.pack(side='left', fill='y', expand=False, padx=(1, 1), pady=(1, 1))
        right_section_frame.pack(side='right', fill='both', expand=True, padx=(1, 1), pady=(1, 1))
        self.images_frame.pack(side='top', fill='both', expand=True, padx=(1, 1), pady=(1, 1))
        self.console_frame.pack(side='bottom', fill='both', expand=True, padx=(1, 1), pady=(1, 1))
        
        self.setup_console()
        self.root.configure(bg='#e6e6e6')
        self.create_image_size_radio_buttons(controls_frame)
        self.setup_image_frames()
        self.create_sliders_and_buttons(controls_frame)

    def setup_console(self):
        self.console = tk.Text(self.console_frame, height=20, bg='black', fg='white')
        self.console.pack(fill='both', expand=True)

    def bmp_to_png(self, bmp_path):
        new_size = (201, 300)
        png_path = bmp_path.replace('.bmp', '.png')
        with Image.open(bmp_path) as img:
            img = img.resize(new_size, Image.LANCZOS)
            img.save(png_path, 'PNG')
        return png_path

    def update_output_image(self, function_name):
        output_image_path = f"out/{function_name}.bmp"
        output_image_png_path = self.bmp_to_png(output_image_path)
        try:
            output_photo = tk.PhotoImage(file=output_image_png_path)
            self.output_label.configure(image=output_photo)
            self.output_label.image = output_photo
        except tk.TclError as e:
            self.console.insert(tk.END, f"Failed to load output image: {e}\n")
            self.console.see(tk.END)

    def run_command(self, arg):
        self.root.update_idletasks()

        self.console.delete("1.0", tk.END)
        self.console.insert(tk.END, "Executing...\n")
        self.console.see(tk.END)

        self.root.update_idletasks()

        command = f"make run sigma={self.sigma.get()} boxSize={self.boxSize.get()} motionLength={self.motionLength.get()} bucketFillThreshold={self.bucketFillThreshold.get()} bucketFillX={self.bucketFillX.get()} bucketFillY={self.bucketFillY.get()} resizeWidthBilinear={self.resizeWidthBilinear.get()} resizeHeightBilinear={self.resizeHeightBilinear.get()} resizeWidthBicubic={self.resizeWidthBicubic.get()} resizeHeightBicubic={self.resizeHeightBicubic.get()} resizeWidthNearestNeighbor={self.resizeWidthNearestNeighbor.get()} resizeHeightNearestNeighbor={self.resizeHeightNearestNeighbor.get()} inputImageSize={self.inputImageSize.get()} function={arg}"
        print(f"make run sigma={self.sigma.get()} boxSize={self.boxSize.get()} motionLength={self.motionLength.get()} bucketFillThreshold={self.bucketFillThreshold.get()} bucketFillX={self.bucketFillX.get()} bucketFillY={self.bucketFillY.get()} resizeWidthBilinear={self.resizeWidthBilinear.get()} resizeHeightBilinear={self.resizeHeightBilinear.get()} resizeWidthBicubic={self.resizeWidthBicubic.get()} resizeHeightBicubic={self.resizeHeightBicubic.get()} resizeWidthNearestNeighbor={self.resizeWidthNearestNeighbor.get()} resizeHeightNearestNeighbor={self.resizeHeightNearestNeighbor.get()} inputImageSize={self.inputImageSize.get()} function={arg}")
        self.console.delete("1.0", tk.END)
        process = Popen(command, shell=True, stdout=PIPE, stderr=STDOUT, text=True)
        display_output = False 
        while True:
            output = process.stdout.readline()
            if output == '' and process.poll() is not None:
                break
            if "Parsing input image using a single thread..." in output: 
                display_output = True
            if display_output:
                self.console.insert(tk.END, output)
                self.console.see(tk.END)
        self.update_output_image(arg)

    def create_image_size_radio_buttons(self, controls_frame):
        size_label_frame = tk.Frame(controls_frame, bg='#e6e6e6')
        size_label_frame.pack(expand=False, anchor='nw', fill='x')
        
        size_label = tk.Label(size_label_frame, text="Input Image Size:", bg='#e6e6e6')
        size_label.pack(side='left', anchor='w') 
        
        rb_container = tk.Frame(size_label_frame, bg='#e6e6e6')
        rb_container.pack(side='left', padx=5, pady=1) 

        self.inputImageSize = tk.StringVar(value='small') 
        sizes = {"Small": "small", "Medium": "medium", "Large": "large"}
        for text, size in sizes.items():
            rb = tk.Radiobutton(rb_container, text=text, variable=self.inputImageSize, value=size, bg='#e6e6e6')
            rb.pack(side='left', padx=1, pady=1)

    def set_window_position(self, width, height):
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()
        center_x = int((screen_width - width) / 2)
        center_y = int((screen_height - height) / 2) - 50
        self.root.geometry(f"{width}x{height}+{center_x}+{center_y}")

    def setup_image_frames(self):
        self.input_image_path = "in/smallImage.bmp"
        input_image_png_path = self.bmp_to_png(self.input_image_path)
        input_photo = tk.PhotoImage(file=input_image_png_path)
        self.input_label = self.setup_image_frame(self.images_frame, input_photo, "Original")
        self.output_label = self.setup_image_frame(self.images_frame, input_photo, "Modified (Fixed to Original Aspect Ratio)")

    def setup_image_frame(self, parent_frame, photo, text):
        label_frame = tk.Frame(parent_frame, bg='#f0f0f0')
        label_frame.pack(side='left', padx=(1, 1), pady=(25, 0), expand=True, fill='both')
        label = tk.Label(label_frame, image=photo)
        label.pack()
        label.image = photo
        tk.Label(label_frame, text=text, bg='#f0f0f0').pack()
        return label

    def create_sliders_and_buttons(self, controls_frame):
        left_column_frame = tk.Frame(controls_frame, bg='#e6e6e6')
        right_column_frame = tk.Frame(controls_frame, bg='#e6e6e6')

        left_column_frame.pack(side='left', fill='both', expand=True, padx=(1, 1), pady=1)
        right_column_frame.pack(side='right', fill='both', expand=True, padx=(1, 1), pady=1)

        configurations = [
            {"slider_name": "sigma", "label": "Sigma", "function_name": "gaussianBlur", "color": "#B28DFF", "default": 3.0},
            {"slider_name": "boxSize", "label": "Box Size", "function_name": "boxBlur", "color": "#FFD580", "default": 9},
            {"slider_name": "motionLength", "label": "Motion Length", "function_name": "motionBlur", "color": "#80CBC4", "default": 15},
            {"slider_name": "bucketFillThreshold", "label": "Bucket Fill Threshold", "function_name": "bucketFill", "color": "#FFABAB", "default": 75},
            {"slider_name": "bucketFillX", "label": "Bucket Fill X", "function_name": "bucketFill", "color": "#FFABAB", "default": 800},
            {"slider_name": "bucketFillY", "label": "Bucket Fill Y", "function_name": "bucketFill", "color": "#FFABAB", "default": 170},
            {"slider_name": "resizeWidthBilinear", "label": "Resize Width Bilinear", "function_name": "bilinearResize", "color": "#A7FFEB", "default": 500},
            {"slider_name": "resizeHeightBilinear", "label": "Resize Height Bilinear", "function_name": "bilinearResize", "color": "#A7FFEB", "default": 745},
            {"slider_name": "resizeWidthBicubic", "label": "Resize Width Bicubic", "function_name": "bicubicResize", "color": "#FFC4E1", "default": 500},
            {"slider_name": "resizeHeightBicubic", "label": "Resize Height Bicubic", "function_name": "bicubicResize", "color": "#FFC4E1", "default": 745},
            {"slider_name": "resizeWidthNearestNeighbor", "label": "Resize Width Nearest Neighbor", "function_name": "nearestNeighborResize", "color": "#B9FBC0", "default": 500},
            {"slider_name": "resizeHeightNearestNeighbor", "label": "Resize Height Nearest Neighbor", "function_name": "nearestNeighborResize", "color": "#B9FBC0", "default": 745},
        ]

        for i, config in enumerate(configurations):
            if i < len(configurations) // 2:
                self.create_slider_and_button(left_column_frame, **config)
            else:
                self.create_slider_and_button(right_column_frame, **config)

    def create_slider_and_button(self, container_frame, slider_name, label, function_name, color, default):
        slider_frame = tk.Frame(container_frame, bg=color)
        slider_frame.pack(fill='x', expand=True, padx=5, pady=5)

        slider_label = tk.Label(slider_frame, text=label, bg=color)
        slider_label.pack()

        slider = tk.Scale(slider_frame, from_=0, to=default*3, orient=tk.HORIZONTAL, length=200, bg=color)
        slider.set(default)
        slider.pack()

        button = ttk.Button(slider_frame, text=f"Apply {function_name}", command=lambda: self.run_command(function_name))
        button.pack(pady=5)

        setattr(self, slider_name, slider)

def main():
    root = tk.Tk()
    app = ImageProcessorApp(root)
    root.mainloop()

if __name__ == "__main__":
    main()
