import sys
import os
import argparse
import subprocess
# python loader.py path/to/your.dll
def convert_dll_to_header(dll_path, output_header="debug.h"):
    try:
        if not os.path.exists(dll_path):
            print(f"Error: File '{dll_path}' not found.")
            return

        # Read the DLL in binary mode
        with open(dll_path, 'rb') as f:
            data = f.read()

        # Create the variable name based on filename (e.g., test.dll -> test_dll)
        filename = os.path.basename(dll_path)
        var_name = "test_dll" # always use this buffer name because it's hardcoded this way in main.c

        with open(output_header, 'w') as out:
            out.write(f"// Generated from {filename}\n")
            out.write(f"#ifndef {var_name.upper()}_H\n")
            out.write(f"#define {var_name.upper()}_H\n\n")

            out.write(f"unsigned char {var_name}[] = {{\n")

            # Write bytes in 0x00 format, 12 per line
            line_width = 12
            for i in range(0, len(data), line_width):
                chunk = data[i:i + line_width]
                hex_str = ", ".join(f"0x{b:02x}" for b in chunk)
                out.write(f"    {hex_str}")
                if i + line_width < len(data):
                    out.write(",")
                out.write("\n")

            out.write("};\n\n")
            out.write(f"unsigned int {var_name}_len = {len(data)};\n\n")
            out.write("#endif\n")

        print(f"Successfully wrote {len(data)} bytes to {output_header}")

    except Exception as e:
        print(f"An error occurred: {e}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="DLL to C Header converter and Env Var helper")
    parser.add_argument("dll_path", help="Path to the DLL file")

    args = parser.parse_args()

    # Always convert the DLL to header as the primary function
    convert_dll_to_header(args.dll_path)