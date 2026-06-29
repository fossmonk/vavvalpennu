import os
import math
import argparse
from PIL import Image

def calculate_best_fit_bbox(image_path, num_frames, frame_width):
    if not os.path.exists(image_path):
        raise FileNotFoundError(f"Could not find the image at {image_path}")
        
    img = Image.open(image_path)
    if img.mode != 'RGBA':
        img = img.convert('RGBA')
        
    img_width, height = img.size
    
    # Validation check to ensure dimensions match parameters
    expected_width = num_frames * frame_width
    if img_width < expected_width:
        print(f"Warning: Spritesheet width ({img_width}px) is less than expected width ({expected_width}px) based on arguments.")

    # Global tracking variables for the non-transparent pixel boundary envelope
    # Coordinates are calculated RELATIVE to the local frame coordinate space
    min_x, min_y = frame_width, height
    max_x, max_y = 0, 0
    
    # Store all relative non-transparent pixel coordinates across all frames to calculate radius later
    all_relative_pixels = []

    # Step 1: Scan all frames to find the cumulative pixel bounding boundaries
    for f in range(num_frames):
        # Crop out the individual frame using user-specified width
        box = (f * frame_width, 0, (f + 1) * frame_width, height)
        frame = img.crop(box)
        
        # Load alpha channel data
        alpha = frame.split()[-1]
        alpha_pixels = alpha.load()
        
        for y in range(height):
            for x in range(frame_width):
                if alpha_pixels[x, y] > 0:  # Pixel is not fully transparent
                    all_relative_pixels.append((x, y))
                    if x < min_x: min_x = x
                    if x > max_x: max_x = x
                    if y < min_y: min_y = y
                    if y > max_y: max_y = y

    if not all_relative_pixels:
        print("No non-transparent pixels found in the spritesheet.")
        return None

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

    # --- Evaluation ---
    if rect_area <= circle_area:
        bbox_type = "rectangle"
        bbox_params = [int(rect_x), int(rect_y), int(rect_w), int(rect_h)]
    else:
        bbox_type = "circle"
        bbox_params = [int(round(center_x)), int(round(center_y)), int(circle_r)]

    return bbox_type, bbox_params

def main():
    parser = argparse.ArgumentParser(description="Generate a best-fit bounding box or circle configuration from an RGBA PNG spritesheet strip.")
    
    # Mandatory arguments
    parser.add_argument("image", type=str, help="Path to the horizontal spritesheet PNG image file")
    parser.add_argument("count", type=int, help="Number of sprite frames inside the sheet")
    parser.add_argument("width", type=int, help="Width of an individual sprite frame in pixels")
    
    # Optional argument
    parser.add_argument("-o", "--output", type=str, default=None, 
                        help="Output file name path. Defaults to <spritesheet_filename>.bbox if omitted")

    args = parser.parse_args()

    # Determine the default output file name if none given
    if args.output is None:
        base_name, _ = os.path.splitext(args.image)
        output_filename = f"{base_name}.bbox"
    else:
        output_filename = args.output

    try:
        result = calculate_best_fit_bbox(args.image, args.count, args.width)
        if result:
            bbox_type, bbox_params = result
            
            with open(output_filename, "w") as f:
                f.write(f"bbox_type={bbox_type}\n")
                f.write(f"bbox_params={bbox_params}\n")
                
            print(f"Successfully evaluated footprint. Configuration written to: {output_filename}")
            print(f"Result: bbox_type={bbox_type} | bbox_params={bbox_params}")
    except Exception as e:
        print(f"Error processing parameters: {e}")

if __name__ == "__main__":
    main()
