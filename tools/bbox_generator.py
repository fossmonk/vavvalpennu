import os
import math
import argparse
import tkinter as tk
from tkinter import ttk, messagebox
from PIL import Image, ImageTk, ImageDraw

def calculate_best_fit_bbox(image_path, num_frames, frame_width, include_flipped=False):
    if not os.path.exists(image_path):
        raise FileNotFoundError(f"Could not find the image at {image_path}")
        
    img = Image.open(image_path)
    if img.mode != 'RGBA':
        img = img.convert('RGBA')
        
    img_width, height = img.size
    
    expected_width = num_frames * frame_width
    if img_width < expected_width:
        print(f"Warning: Spritesheet width ({img_width}px) is less than expected width ({expected_width}px) based on arguments.")

    all_relative_pixels = []

    for f in range(num_frames):
        box = (f * frame_width, 0, (f + 1) * frame_width, height)
        frame = img.crop(box)
        
        alpha = frame.split()[-1]
        alpha_pixels = alpha.load()
        
        for y in range(height):
            for x in range(frame_width):
                if alpha_pixels[x, y] > 0:
                    all_relative_pixels.append((x, y))
                    if include_flipped:
                        # Include the mirror position relative to the individual frame width
                        all_relative_pixels.append((frame_width - 1 - x, y))

    if not all_relative_pixels:
        print("No non-transparent pixels found in the spritesheet.")
        return "rectangle", [0, 0, frame_width, height]

    # Calculate bounding envelope from the full pixel list
    min_x = min(p[0] for p in all_relative_pixels)
    max_x = max(p[0] for p in all_relative_pixels)
    min_y = min(p[1] for p in all_relative_pixels)
    max_y = max(p[1] for p in all_relative_pixels)

    # --- Rectangle Calculations ---
    rect_x = min_x
    rect_y = min_y
    rect_w = max_x - min_x + 1
    rect_h = max_y - min_y + 1
    rect_area = rect_w * rect_h

    # --- Circle Calculations ---
    center_x = min_x + (rect_w / 2.0)
    center_y = min_y + (rect_h / 2.0)
    
    max_radius_sq = 0
    for px, py in all_relative_pixels:
        dist_sq = (px - center_x) ** 2 + (py - center_y) ** 2
        if dist_sq > max_radius_sq:
            max_radius_sq = dist_sq
            
    circle_r = math.ceil(math.sqrt(max_radius_sq))
    circle_area = math.pi * (circle_r ** 2)

    if rect_area <= circle_area:
        return "rectangle", [int(rect_x), int(rect_y), int(rect_w), int(rect_h)]
    else:
        return "circle", [int(round(center_x)), int(round(center_y)), int(circle_r)]


class InteractiveBBoxEditor:
    def __init__(self, image_path, num_frames, frame_width, initial_type, initial_params, output_path, start_flipped=False):
        self.image_path = image_path
        self.num_frames = num_frames
        self.frame_width = frame_width
        self.output_path = output_path
        
        # Load and process individual frames
        img = Image.open(image_path).convert("RGBA")
        self.frame_height = img.height
        
        self.frames = []
        self.flipped_frames = []
        for f in range(num_frames):
            box = (f * frame_width, 0, (f + 1) * frame_width, self.frame_height)
            cropped = img.crop(box)
            self.frames.append(cropped)
            # Pre-calculate horizontally mirrored variants
            self.flipped_frames.append(cropped.transpose(Image.FLIP_LEFT_RIGHT))
            
        # Create the superimposed frames (normal & flipped)
        self.superimposed_frame = Image.new("RGBA", (frame_width, self.frame_height), (0, 0, 0, 0))
        for f in self.frames:
            self.superimposed_frame = Image.alpha_composite(self.superimposed_frame, f)
            
        self.superimposed_flipped_frame = self.superimposed_frame.transpose(Image.FLIP_LEFT_RIGHT)
            
        # State variables
        self.current_frame_index = 0  
        self.bbox_type = initial_type
        self.polygon_points = []
        self.zoom_factor = 4 
        
        # Parse initial parameters
        self.rect_x, self.rect_y, self.rect_w, self.rect_h = 0, 0, frame_width, self.frame_height
        self.circle_x, self.circle_y, self.circle_r = int(frame_width/2), int(self.frame_height/2), int(frame_width/4)
        
        if initial_type == "rectangle":
            self.rect_x, self.rect_y, self.rect_w, self.rect_h = initial_params
        elif initial_type == "circle":
            self.circle_x, self.circle_y, self.circle_r = initial_params

        # Build GUI Window
        self.root = tk.Tk()
        self.root.title("Vavvalpennu BBox Editor")
        
        # Enable Window Resizing and Expansion
        self.root.resizable(True, True)
        self.root.columnconfigure(1, weight=1)
        self.root.rowconfigure(0, weight=1)
        
        # Main Layout Frame Splitting using Grids
        control_frame = ttk.Frame(self.root, padding=10)
        control_frame.grid(row=0, column=0, sticky="ns")
        
        canvas_container = ttk.Frame(self.root, padding=10)
        canvas_container.grid(row=0, column=1, sticky="nsew")
        canvas_container.columnconfigure(0, weight=1)
        canvas_container.rowconfigure(0, weight=1)
        
        # BBox Type Panel 
        ttk.Label(control_frame, text="Bounding Box Type:", font=('Helvetica', 10, 'bold')).pack(anchor=tk.W, pady=5)
        self.type_var = tk.StringVar(value=self.bbox_type)
        for t in ["rectangle", "circle", "polygon"]:
            ttk.Radiobutton(control_frame, text=t.capitalize(), value=t, variable=self.type_var, command=self.on_type_change).pack(anchor=tk.W, padx=10)
            
        ttk.Separator(control_frame, orient='horizontal').pack(fill='x', pady=10)
        
        # Frame Navigation Controls
        ttk.Label(control_frame, text="Frame Controls:", font=('Helvetica', 10, 'bold')).pack(anchor=tk.W, pady=5)
        nav_button_frame = ttk.Frame(control_frame)
        nav_button_frame.pack(fill=tk.X, pady=2)
        
        self.btn_prev = ttk.Button(nav_button_frame, text="◀", width=4, command=self.prev_frame)
        self.btn_prev.pack(side=tk.LEFT, padx=2)
        
        self.frame_label = ttk.Label(nav_button_frame, text="", width=12, anchor=tk.CENTER)
        self.frame_label.pack(side=tk.LEFT, expand=True, fill=tk.X)
        
        self.btn_next = ttk.Button(nav_button_frame, text="▶", width=4, command=self.next_frame)
        self.btn_next.pack(side=tk.LEFT, padx=2)
        
        self.superimpose_var = tk.BooleanVar(value=False)
        self.chk_superimpose = ttk.Checkbutton(control_frame, text="Superimpose All Frames",
                                               variable=self.superimpose_var, command=self.toggle_superimpose)
        self.chk_superimpose.pack(anchor=tk.W, pady=5, padx=5)
        
        # Horizontally Flipped Toggle Functionality
        self.flip_var = tk.BooleanVar(value=start_flipped)
        self.chk_flip = ttk.Checkbutton(control_frame, text="Include Horizontally Flipped",
                                        variable=self.flip_var, command=self.render_preview)
        self.chk_flip.pack(anchor=tk.W, pady=5, padx=5)
        
        ttk.Separator(control_frame, orient='horizontal').pack(fill='x', pady=10)

        # Canvas Zoom Scale Dropdown
        ttk.Label(control_frame, text="Display Zoom Scaling:", font=('Helvetica', 10, 'bold')).pack(anchor=tk.W, pady=5)
        self.zoom_var = tk.StringVar(value="4x")
        self.zoom_combo = ttk.Combobox(control_frame, textvariable=self.zoom_var, values=["1x", "2x", "4x", "6x", "8x", "10x"], state="readonly", width=10)
        self.zoom_combo.pack(anchor=tk.W, padx=10, pady=2)
        self.zoom_combo.bind("<<ComboboxSelected>>", self.on_zoom_change)
        
        ttk.Separator(control_frame, orient='horizontal').pack(fill='x', pady=10)
        
        # Parameters Configuration Panel
        self.param_frame = ttk.LabelFrame(control_frame, text=" Parameters (Original Pixels) ", padding=10)
        self.param_frame.pack(fill=tk.X, pady=5)
        self.entries = {}
        
        self.instruction_label = ttk.Label(control_frame, text="", wraplength=180, justify=tk.LEFT, foreground="blue")
        self.instruction_label.pack(fill=tk.X, pady=10)
        
        ttk.Button(control_frame, text="Save & Close", command=self.save_and_exit).pack(fill=tk.X, side=tk.BOTTOM, pady=5)
        
        # Interactive Canvas Viewport and Scrollbars
        xscroll = ttk.Scrollbar(canvas_container, orient=tk.HORIZONTAL)
        xscroll.grid(row=1, column=0, sticky="ew")
        yscroll = ttk.Scrollbar(canvas_container, orient=tk.VERTICAL)
        yscroll.grid(row=0, column=1, sticky="ns")

        self.canvas = tk.Canvas(canvas_container, bg="#333333", xscrollcommand=xscroll.set, yscrollcommand=yscroll.set)
        self.canvas.grid(row=0, column=0, sticky="nsew")
        
        xscroll.config(command=self.canvas.xview)
        yscroll.config(command=self.canvas.yview)
        
        # Bind Canvas Mouse Actions
        self.canvas.bind("<Button-1>", self.on_canvas_click)
        self.canvas.bind("<B1-Motion>", self.on_canvas_drag)
        
        # Adjust scrolling region when layout configuration changes
        self.canvas.bind("<Configure>", lambda event: self.resize_canvas_dimensions())

        self.update_frame_label()
        self.update_ui_fields()
        self.resize_canvas_dimensions()
        self.render_preview()
        
    def resize_canvas_dimensions(self):
        w = self.frame_width * self.zoom_factor
        h = self.frame_height * self.zoom_factor
        self.canvas.config(scrollregion=(0, 0, w, h))

    def on_zoom_change(self, event=None):
        zoom_str = self.zoom_var.get()
        self.zoom_factor = int(zoom_str.replace("x", ""))
        self.resize_canvas_dimensions()
        self.render_preview()

    def update_frame_label(self):
        if self.superimpose_var.get():
            self.frame_label.config(text="Superimposed")
            self.btn_prev.state(['disabled'])
            self.btn_next.state(['disabled'])
        else:
            self.frame_label.config(text=f"Frame {self.current_frame_index + 1} / {self.num_frames}")
            self.btn_prev.state(['!disabled'])
            self.btn_next.state(['!disabled'])

    def prev_frame(self):
        if not self.superimpose_var.get():
            self.current_frame_index = (self.current_frame_index - 1) % self.num_frames
            self.update_frame_label()
            self.render_preview()

    def next_frame(self):
        if not self.superimpose_var.get():
            self.current_frame_index = (self.current_frame_index + 1) % self.num_frames
            self.update_frame_label()
            self.render_preview()

    def toggle_superimpose(self):
        self.update_frame_label()
        self.render_preview()

    def on_type_change(self):
        self.bbox_type = self.type_var.get()
        self.update_ui_fields()
        self.render_preview()
        
    def update_ui_fields(self):
        for widget in self.param_frame.winfo_children():
            widget.destroy()
        self.entries.clear()
        
        if self.bbox_type == "rectangle":
            self.instruction_label.config(text="💡 Click and drag directly on the canvas to draw a rectangle.")
            fields = [("X:", self.rect_x), ("Y:", self.rect_y), ("Width:", self.rect_w), ("Height:", self.rect_h)]
        elif self.bbox_type == "circle":
            self.instruction_label.config(text="💡 Click to position center. Drag outwards to set radius size.")
            fields = [("Center X:", self.circle_x), ("Center Y:", self.circle_y), ("Radius:", self.circle_r)]
        else: # Polygon
            self.instruction_label.config(text="💡 Left-Click to drop sequential points.\nRight-Click anywhere to reset points.")
            fields = []
            ttk.Label(self.param_frame, text=f"Points Added: {len(self.polygon_points)}").pack()
            self.root.bind("<Button-3>", self.clear_polygon)
            
        for label_text, val in fields:
            row = ttk.Frame(self.param_frame)
            row.pack(fill=tk.X, pady=2)
            ttk.Label(row, text=label_text, width=10).pack(side=tk.LEFT)
            ent = ttk.Entry(row, width=8)
            ent.insert(0, str(int(val)))
            ent.pack(side=tk.RIGHT, expand=True, fill=tk.X)
            ent.bind("<Return>", self.on_text_field_update)
            self.entries[label_text] = ent

    def on_text_field_update(self, event=None):
        try:
            if self.bbox_type == "rectangle":
                self.rect_x = int(self.entries["X:"].get())
                self.rect_y = int(self.entries["Y:"].get())
                self.rect_w = int(self.entries["Width:"].get())
                self.rect_h = int(self.entries["Height:"].get())
            elif self.bbox_type == "circle":
                self.circle_x = int(self.entries["Center X:"].get())
                self.circle_y = int(self.entries["Center Y:"].get())
                self.circle_r = int(self.entries["Radius:"].get())
            self.render_preview()
        except ValueError:
            pass

    def get_unscaled_coordinates(self, event):
        # Maps coordinates back down to native resolution
        canvas_x = self.canvas.canvasx(event.x)
        canvas_y = self.canvas.canvasy(event.y)
        
        orig_x = int(canvas_x / self.zoom_factor)
        orig_y = int(canvas_y / self.zoom_factor)
        
        orig_x = max(0, min(orig_x, self.frame_width - 1))
        orig_y = max(0, min(orig_y, self.frame_height - 1))
        return orig_x, orig_y

    def on_canvas_click(self, event):
        orig_x, orig_y = self.get_unscaled_coordinates(event)
        
        if self.bbox_type == "rectangle":
            self.rect_x, self.rect_y = orig_x, orig_y
            self.rect_w, self.rect_h = 1, 1
        elif self.bbox_type == "circle":
            self.circle_x, self.circle_y = orig_x, orig_y
        elif self.bbox_type == "polygon":
            self.polygon_points.append(orig_x)
            self.polygon_points.append(orig_y)
            self.update_ui_fields()
        self.render_preview()

    def on_canvas_drag(self, event):
        orig_x, orig_y = self.get_unscaled_coordinates(event)
        
        if self.bbox_type == "rectangle":
            self.rect_w = max(1, orig_x - self.rect_x)
            self.rect_h = max(1, orig_y - self.rect_y)
        elif self.bbox_type == "circle":
            self.circle_r = max(1, int(math.hypot(orig_x - self.circle_x, orig_y - self.circle_y)))
        self.update_ui_fields()
        self.render_preview()

    def clear_polygon(self, event=None):
        self.polygon_points = []
        self.update_ui_fields()
        self.render_preview()

    def render_preview(self):
        # Determine the correct configuration blend
        if self.superimpose_var.get():
            base_frame = self.superimposed_frame.copy()
            if self.flip_var.get():
                # Blend superimposed normal sprites with superimposed flipped sprites
                base_frame = Image.alpha_composite(base_frame, self.superimposed_flipped_frame)
        else:
            base_frame = self.frames[self.current_frame_index].copy()
            if self.flip_var.get():
                # Blend individual frame with its corresponding flipped frame
                base_frame = Image.alpha_composite(base_frame, self.flipped_frames[self.current_frame_index])
            
        draw = ImageDraw.Draw(base_frame)
        color_outline = (0, 255, 0, 255) 
        
        if self.bbox_type == "rectangle":
            draw.rectangle([self.rect_x, self.rect_y, self.rect_x + self.rect_w, self.rect_y + self.rect_h], outline=color_outline, width=1)
        elif self.bbox_type == "circle":
            draw.ellipse([self.circle_x - self.circle_r, self.circle_y - self.circle_r, self.circle_x + self.circle_r, self.circle_y + self.circle_r], outline=color_outline, width=1)
        elif self.bbox_type == "polygon" and len(self.polygon_points) >= 4:
            pts = [(self.polygon_points[i], self.polygon_points[i+1]) for i in range(0, len(self.polygon_points), 2)]
            draw.polygon(pts, outline=color_outline, width=1)
            
        scaled_w = self.frame_width * self.zoom_factor
        scaled_h = self.frame_height * self.zoom_factor
        
        # Pixels remain sharp (nearest-neighbor) regardless of frame size
        zoomed_preview = base_frame.resize((scaled_w, scaled_h), Image.NEAREST)
            
        self.tk_img = ImageTk.PhotoImage(zoomed_preview)
        self.canvas.delete("all")
        self.canvas.create_image(0, 0, anchor=tk.NW, image=self.tk_img)

    def save_and_exit(self):
        if self.bbox_type == "rectangle":
            final_params = [int(self.rect_x), int(self.rect_y), int(self.rect_w), int(self.rect_h)]
        elif self.bbox_type == "circle":
            final_params = [int(self.circle_x), int(self.circle_y), int(self.circle_r)]
        else: # Polygon
            if len(self.polygon_points) < 6:
                messagebox.showerror("Error", "A polygon requires at least 3 points (6 coordinate positions).")
                return
            final_params = [int(p) for p in self.polygon_points]

        with open(self.output_path, "w") as f:
            f.write(f"bbox_type={self.bbox_type}\n")
            f.write(f"bbox_params={final_params}\n")
            
        print(f"Successfully saved configuration: {self.output_path}")
        print(f"Result: bbox_type={self.bbox_type} | bbox_params={final_params}")
        self.root.destroy()

    def run(self):
        self.root.mainloop()


def main():
    parser = argparse.ArgumentParser(description="Generate a best-fit bounding box or circle configuration from an RGBA PNG spritesheet strip.")
    
    # Arguments
    parser.add_argument("image", type=str, help="Path to the horizontal spritesheet PNG image file")
    parser.add_argument("count", type=int, help="Number of sprite frames inside the sheet")
    parser.add_argument("width", type=int, help="Width of an individual sprite frame in pixels")
    parser.add_argument("-o", "--output", type=str, default=None, 
                        help="Output file name path. Defaults to <spritesheet_filename>.bbox if omitted")
    parser.add_argument("-i", "--interactive", action="store_true",
                        help="Launch visual window workspace to manually adjust parameters or draw polygons")
    parser.add_argument("-f", "--flipped", action="store_true",
                        help="Include horizontally flipped versions of frames when auto-calculating best fit boundaries")

    args = parser.parse_args()

    if args.output is None:
        base_name, _ = os.path.splitext(args.image)
        output_filename = f"{base_name}.bbox"
    else:
        output_filename = args.output

    try:
        result = calculate_best_fit_bbox(args.image, args.count, args.width, include_flipped=args.flipped)
        if result:
            bbox_type, bbox_params = result
            
            if args.interactive:
                print("Launching interactive editor workspace...")
                editor = InteractiveBBoxEditor(args.image, args.count, args.width, bbox_type, bbox_params, output_filename, start_flipped=args.flipped)
                editor.run()
            else:
                with open(output_filename, "w") as f:
                    f.write(f"bbox_type={bbox_type}\n")
                    f.write(f"bbox_params={bbox_params}\n")
                    
                print(f"Successfully evaluated footprint. Configuration written to: {output_filename}")
                print(f"Result: bbox_type={bbox_type} | bbox_params={bbox_params}")
            
    except Exception as e:
        print(f"Error processing parameters: {e}")

if __name__ == "__main__":
    main()