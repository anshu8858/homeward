#!/usr/bin/env python3
import sys
import os

try:
    from PIL import Image
except ImportError:
    print("Pillow library not found. Install via 'pip install Pillow'")
    sys.exit(1)

def hex_to_rgb(hex_str):
    hex_str = hex_str.lstrip('#')
    return tuple(int(hex_str[i:i+2], 16) for i in (0, 2, 4))

def interpolate_color(c1, c2, factor):
    return tuple(int(c1[i] + (c2[i] - c1[i]) * factor) for i in range(3))

def generate_gradient_palette(colors, output_filename, width=256, height=16):
    """
    Generates a 1D gradient texture (represented as a 2D image) blending between the provided colors.
    Used for the unlit material palette-swapping style in Homeward.
    """
    img = Image.new('RGB', (width, height))
    pixels = img.load()
    
    num_segments = len(colors) - 1
    if num_segments == 0:
        for x in range(width):
            for y in range(height):
                pixels[x, y] = colors[0]
        img.save(output_filename)
        return

    segment_width = width / num_segments
    
    for x in range(width):
        segment_idx = min(int(x / segment_width), num_segments - 1)
        factor = (x - (segment_idx * segment_width)) / segment_width
        
        c1 = colors[segment_idx]
        c2 = colors[segment_idx + 1]
        
        blended_color = interpolate_color(c1, c2, factor)
        
        for y in range(height):
            pixels[x, y] = blended_color
            
    img.save(output_filename)
    print(f"Palette saved to {output_filename}")

if __name__ == "__main__":
    print("Generating Homeward M2 Textures...")
    
    # M2: Grandmother's House (Warm, nostalgic, sepia to sunset orange to deep brown)
    grandmas_house_colors = [
        hex_to_rgb("#FFEEDB"), # Off-white light
        hex_to_rgb("#F4A261"), # Sunset Orange
        hex_to_rgb("#E76F51"), # Burnt Sienna
        hex_to_rgb("#2A1B18"), # Deep Brown Shadow
    ]
    
    output_path = os.path.join(os.path.dirname(__file__), "..", "Palette_GrandmasHouse.png")
    generate_gradient_palette(grandmas_house_colors, output_path)
