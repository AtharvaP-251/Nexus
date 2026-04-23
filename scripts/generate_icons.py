import os
from PIL import Image, ImageOps, ImageDraw

def create_round_icon(img):
    """Creates a circular version of the input image."""
    img = img.convert("RGBA")
    size = img.size
    # Create a high-res mask for smoother edges
    mask_size = (size[0] * 4, size[1] * 4)
    mask = Image.new('L', mask_size, 0)
    draw = ImageDraw.Draw(mask)
    draw.ellipse((0, 0) + mask_size, fill=255)
    mask = mask.resize(size, Image.Resampling.LANCZOS)
    
    output = img.copy()
    output.putalpha(mask)
    return output

def generate_icons(source_path, res_path):
    sizes = {
        'mipmap-mdpi': 48,
        'mipmap-hdpi': 72,
        'mipmap-xhdpi': 96,
        'mipmap-xxhdpi': 144,
        'mipmap-xxxhdpi': 192,
    }
    
    if not os.path.exists(source_path):
        print(f"Error: Source image not found at {source_path}")
        return

    with Image.open(source_path) as img:
        # Ensure we work with RGBA if we want transparency later
        img = img.convert("RGBA")
        
        # Square and Round icons
        for folder, size in sizes.items():
            target_dir = os.path.join(res_path, folder)
            os.makedirs(target_dir, exist_ok=True)
            
            # Resize for square icon
            resized = img.resize((size, size), Image.Resampling.LANCZOS)
            target_path = os.path.join(target_dir, 'ic_launcher.png')
            resized.save(target_path)
            
            # Clean up potentially conflicting webp files
            webp_path = target_path.replace('.png', '.webp')
            if os.path.exists(webp_path):
                os.remove(webp_path)
                print(f"Removed conflicting: {webp_path}")

            # Create and save round icon
            round_img = create_round_icon(resized)
            round_target_path = os.path.join(target_dir, 'ic_launcher_round.png')
            round_img.save(round_target_path)
            
            # Clean up potentially conflicting round webp files
            round_webp_path = round_target_path.replace('.png', '.webp')
            if os.path.exists(round_webp_path):
                os.remove(round_webp_path)
                print(f"Removed conflicting: {round_webp_path}")
            
            print(f"Generated icons for {folder} ({size}x{size})")
        
        # Play Store icon (512x512)
        store_icon = img.resize((512, 512), Image.Resampling.LANCZOS)
        store_icon.save(os.path.join(res_path, 'ic_launcher-web.png'))
        print("Generated 512x512 store icon (ic_launcher-web.png)")

        # Adaptive icon foreground layers (108dp base)
        # mdpi=108, hdpi=162, xhdpi=216, xxhdpi=324, xxxhdpi=432
        foreground_sizes = {
            'drawable-mdpi': 108,
            'drawable-hdpi': 162,
            'drawable-xhdpi': 216,
            'drawable-xxhdpi': 324,
            'drawable-xxxhdpi': 432,
        }
        
        for folder, size in foreground_sizes.items():
            target_dir = os.path.join(res_path, folder)
            os.makedirs(target_dir, exist_ok=True)
            
            # Create 108dp transparent canvas
            canvas = Image.new("RGBA", (size, size), (0, 0, 0, 0))
            # Logo fits in safe zone (~70% of 108dp)
            logo_size = int(size * 0.72)
            logo_resized = img.resize((logo_size, logo_size), Image.Resampling.LANCZOS)
            offset = (size - logo_size) // 2
            canvas.paste(logo_resized, (offset, offset), logo_resized)
            
            canvas.save(os.path.join(target_dir, 'ic_launcher_foreground.png'))
            # Clean up old XML if it exists in the same folder
            xml_conflict = os.path.join(target_dir, 'ic_launcher_foreground.xml')
            if os.path.exists(xml_conflict):
                os.remove(xml_conflict)
            
        print("Generated adaptive foreground layers (.png) in drawable folders")

        # Ensure anydpi XMLs exist and point to the foreground
        anydpi_dir = os.path.join(res_path, 'mipmap-anydpi')
        os.makedirs(anydpi_dir, exist_ok=True)
        
        adaptive_xml_content = """<?xml version="1.0" encoding="utf-8"?>
<adaptive-icon xmlns:android="http://schemas.android.com/apk/res/android">
    <background android:drawable="@drawable/ic_launcher_background" />
    <foreground android:drawable="@drawable/ic_launcher_foreground" />
    <monochrome android:drawable="@drawable/ic_launcher_foreground" />
</adaptive-icon>"""

        for name in ['ic_launcher.xml', 'ic_launcher_round.xml']:
            with open(os.path.join(anydpi_dir, name), 'w') as f:
                f.write(adaptive_xml_content)
        
        print("Updated adaptive icon XMLs in mipmap-anydpi")

if __name__ == "__main__":
    # Base directory is assumed to be the project root
    PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    source = os.path.join(PROJECT_ROOT, "dox", "Nexus N Logo.png")
    res = os.path.join(PROJECT_ROOT, "app", "src", "main", "res")
    
    print(f"Source: {source}")
    print(f"Res Target: {res}")
    
    generate_icons(source, res)
