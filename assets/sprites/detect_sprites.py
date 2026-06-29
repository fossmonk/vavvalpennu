import os
import subprocess
from pathlib import Path
from PIL import Image

# Path to the bbox generator script (assumed to be in the same directory as this script)
SCRIPT_DIR = Path(__file__).parent
BBOX_SCRIPT = SCRIPT_DIR / "bbox_generator.py"

# Candidate frame widths to try (most sprites are 256px wide, some are 64px)
CANDIDATE_WIDTHS = [256, 128, 64, 32]

def infer_frame_params(image_path: Path):
    with Image.open(image_path) as img:
        total_width, height = img.size
    # Try each candidate width; pick the first that divides total_width evenly and yields >1 frames
    for fw in CANDIDATE_WIDTHS:
        if total_width % fw == 0:
            count = total_width // fw
            if count > 1:
                return count, fw
    # Fallback: treat the whole sheet as a single frame
    return 1, total_width

def process_sprite_sheet(png_path: Path):
    count, frame_width = infer_frame_params(png_path)
    cmd = ["python", str(BBOX_SCRIPT), str(png_path), str(count), str(frame_width)]
    print(f"Running bbox_generator on {png_path} with count={count}, frame_width={frame_width}")
    subprocess.run(cmd, check=True)

def main():
    sprites_root = SCRIPT_DIR  # assuming this script lives in assets/sprites
    for root, _, files in os.walk(sprites_root):
        for f in files:
            if f.lower().endswith('.png'):
                png_path = Path(root) / f
                # Skip the script itself if placed in the same folder
                if png_path.name == "detect_sprites.py":
                    continue
                process_sprite_sheet(png_path)

if __name__ == "__main__":
    main()
