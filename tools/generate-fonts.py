from fontTools.ttLib import TTFont
from fontTools.subset import Subsetter, Options
import json
from pprint import pprint
import io


def parse_character_set(char_set_string):
    """
    Parse a string of characters and ranges into a set of Unicode code points.

    Input format: "10,11,0x20-0x7F,0xB0,0x2022,0xF10C2"
    Returns: A set of integers representing Unicode code points.
    """
    char_set = set()
    entries = char_set_string.split(",")

    for entry in entries:
        if "-" in entry:
            # Handle ranges like 0x20-0x7F
            start, end = entry.split("-")
            start = int(start, 0)
            end = int(end, 0)
            char_set.update(range(start, end + 1))
        else:
            # Handle single characters like 10 or 0x20
            char_set.add(int(entry, 0))

    return char_set


def create_subset_font(input_font_path, char_set_string):
    char_set = parse_character_set(char_set_string)

    font = TTFont(input_font_path)

    subsetter = Subsetter(options=Options())
    subsetter.unicodes_requested.update(char_set)
    subsetter.subset(font)

    font_buffer = io.BytesIO()
    font.save(font_buffer)
    font_buffer.seek(0)
    return font_buffer.getvalue()


with open("generate-fonts.json") as f:
    config = json.load(f)

with open("../main/ttffonts.c", "w") as cf, open("../main/ttffonts.h", "w") as hf:
    hf.write("#pragma once\n")
    hf.write("\n")
    hf.write("#include <stdint.h>\n")
    hf.write("#include <stddef.h>\n")
    hf.write("\n")
    hf.write("#ifdef __cplusplus\n")
    hf.write('extern "C" {\n')
    hf.write("#endif\n")
    hf.write("\n")

    cf.write('#include "ttffonts.h"\n')
    cf.write("\n")

    for entry in config:
        print(f"Generating {entry["name"]}...")

        font_data = create_subset_font(entry["font"], entry["range"])

        hf.write(f"extern const uint8_t {entry["name"]}[];\n")
        hf.write(f"extern size_t {entry["name"]}_size;\n")
        hf.write("\n")

        cf.write(f"const uint8_t {entry["name"]}[] = {{\n")

        count = 0

        for b in font_data:
            if count == 0:
                cf.write("    ")
            cf.write(f"0x{b:02x}, ")
            count += 1
            if count == 20:
                cf.write("\n")
                count = 0

        cf.write("\n};\n")
        cf.write("\n")
        cf.write(f"size_t {entry["name"]}_size = sizeof({entry["name"]});\n")
        cf.write("\n")

    hf.write("#ifdef __cplusplus\n")
    hf.write("};\n")
    hf.write("#endif\n")
