'use strict';

function is_intel_hex(str) {
  const intelHex = /^:[0-9a-fA-F][0-9a-fA-F]/;

  return str.match(intelHex);
}

function hex_to_bin(intel_hex_data) {
    return intel_hex.parse(intel_hex_data).data;
}

async function delay(milliseconds) {
    await new Promise(resolve => {setTimeout(() => {resolve()}, milliseconds)});
}
