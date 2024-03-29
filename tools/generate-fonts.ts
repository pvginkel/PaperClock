import { execSync } from 'child_process';
import * as fs from 'fs';
import * as yaml from 'js-yaml';
import { exit } from 'process';

for (const fileName of fs.readdirSync("..")) {
  if (/^lv_font_.*\.c$/.test(fileName)) {
    fs.rmSync(`../src/${fileName}`);
  }
}

const fonts: [{ name: string; size: number | [number]; fonts: [{ file: string; range: string }] }] = <any>(
  yaml.load(fs.readFileSync('generate-fonts.yaml', 'utf-8').replace(/^\s*\/\/.*$/gm, ''))
);

let fontNames = [];

for (const font of fonts) {
  let sizes = Array.isArray(font.size) ? font.size : [font.size];

  for (const size of sizes) {
    let name = font.name.replace(/\{size\}/g, `${size}`);
    fontNames.push(name);

    console.log();
    console.log(`Generating ${name}`);
    console.log();

    let args = `--no-compress --no-prefilter --bpp 4 --size ${size} --format lvgl -o "../src/${name}.c" --force-fast-kern-format --lv-include lvgl.h `;

    for (const file of font.fonts) {
      args += `--font "fonts/${file.file}" --range "${file.range}" `;
    }

    if (process.platform === 'win32') {
      console.log(execSync(`call ./node_modules/.bin/lv_font_conv.cmd ${args}`, { encoding: 'utf-8' }));
    } else {
      console.log(execSync(`./node_modules/.bin/lv_font_conv ${args}`, { encoding: 'utf-8' }));
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
