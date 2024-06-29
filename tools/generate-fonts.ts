import { execSync } from 'child_process';
import * as fs from 'fs';
import * as yaml from 'js-yaml';
import { exit } from 'process';

for (const fileName of fs.readdirSync('..')) {
  if (/^lv_font_.*\.c$/.test(fileName)) {
    fs.rmSync(`../src/${fileName}`);
  }
}

const fonts: [
  { name: string; size: undefined | number | [number]; fonts: [{ file: string; range?: string; icons?: [{ name: string; icon: string; image?: string }] }] }
] = <any>yaml.load(fs.readFileSync('generate-fonts.yaml', 'utf-8').replace(/^\s*\/\/.*$/gm, ''));

let fontNames = [];

for (const font of fonts) {
  if (font.size) {
    let sizes = Array.isArray(font.size) ? font.size : [font.size];

    for (const size of sizes) {
      let name = font.name.replace(/\{size\}/g, `${size}`);
      fontNames.push(name);

      console.log();
      console.log(`Generating ${name}`);
      console.log();

      let args = `--no-compress --no-prefilter --bpp 4 --size ${size} --format lvgl -o "../src/${name}.c" --force-fast-kern-format --lv-include lvgl.h `;

      for (const file of font.fonts) {
        let range = file.range;

        if (!range) {
          if (!file.icons) {
            throw 'Either range or icons must be provided';
          }

          range = '';

          for (const icon of file.icons) {
            if (range) {
              range += ',';
            }
            range += `0x${icon.icon}`;
          }
        }

        args += `--font "fonts/${file.file}" --range "${range}" `;
      }

      if (process.platform === 'win32') {
        console.log(execSync(`call ./node_modules/.bin/lv_font_conv.cmd ${args}`, { encoding: 'utf-8' }));
      } else {
        console.log(execSync(`./node_modules/.bin/lv_font_conv ${args}`, { encoding: 'utf-8' }));
      }
    }
  }
}

let header = '#pragma once\n\n';
header += '#ifdef __cplusplus\n';
header += 'extern "C" {\n';
header += '#endif\n\n';

for (const name of fontNames) {
  header += `LV_FONT_DECLARE(${name});\n`;
}

header += '\n';
header += '#ifdef __cplusplus\n';
header += '}\n';
header += '#endif\n';

fs.writeFileSync('../src/fonts.h', header);

let icons = '#pragma once\n\n';

for (const font of fonts) {
  for (const file of font.fonts) {
    if (file.icons) {
      for (const icon of file.icons) {
        icons += `#define ${icon.name.replace(/-/g, '_').toUpperCase()} "\\U${icon.icon}"\n`;
      }
    }
  }
}

icons += '\n';
icons += 'const char* classify_weather_image(const string& image);\n';

fs.writeFileSync('../src/mdi-icons.h', icons);

let body = '#include "includes.h"\n';
body += '#include "mdi-icons.h"\n\n';
body += 'const char* classify_weather_image(const string& image) {\n';

for (const font of fonts) {
  for (const file of font.fonts) {
    if (file.icons) {
      for (const icon of file.icons) {
        if (icon.image) {
          for (const image of icon.image.split(',')) {
            body += `  if (image == "${image}") {\n`;
            body += `    return ${icon.name.replace(/-/g, '_').toUpperCase()};\n`;
            body += '  }\n';
          }
        }
      }
    }
  }
}

body += '  return nullptr;\n';
body += '}\n';

fs.writeFileSync('../src/mdi-icons.cpp', body);
