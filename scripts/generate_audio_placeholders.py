#!/usr/bin/env python3
import os
import wave
import struct
import math

def generate_wav(filename, duration_seconds=0.1, freq=440.0, sample_rate=44100):
    # Generates a simple mono sine wave (or silence if freq=0)
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1) # Mono
        wav_file.setsampwidth(2) # 16-bit
        wav_file.setframerate(sample_rate)
        
        num_samples = int(duration_seconds * sample_rate)
        
        for i in range(num_samples):
            if freq > 0:
                # Sine wave with volume falloff
                volume = min(1.0, 1.0 - (i / num_samples))
                value = int(volume * 16000.0 * math.sin(2.0 * math.pi * freq * i / sample_rate))
            else:
                value = 0
            
            data = struct.pack('<h', value)
            wav_file.writeframesraw(data)
    
    print(f"Generated {os.path.basename(filename)}")

if __name__ == "__main__":
    out_dir = os.path.join(os.path.dirname(__file__), "..", "Content", "Audio")
    os.makedirs(out_dir, exist_ok=True)
    
    print("Generating M2 Audio Placeholders...")
    
    # 4 BGM Loop Placeholders (silent, 1 second each for small file size)
    for i in range(1, 5):
        generate_wav(os.path.join(out_dir, f"BGM_Motif_{i}.wav"), duration_seconds=1.0, freq=0)
        
    # 20 SFX Placeholders (short beeps of varying frequencies)
    sfx_types = [
        ("Pickup", 440.0), 
        ("Drop", 220.0), 
        ("Snap", 880.0), 
        ("Rotate", 660.0),
        ("Invalid", 110.0)
    ]
    
    for name, base_freq in sfx_types:
        for variation in range(1, 5): # 4 variations per SFX type
            generate_wav(os.path.join(out_dir, f"SFX_{name}_{variation}.wav"), duration_seconds=0.15, freq=base_freq + (variation*20))
            
    print("Done.")
