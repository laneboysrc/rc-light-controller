'use strict';

let stdin_buffer = '';


function make_crlf_visible(string) {
  const cr = /\r/g;
  const lf = /\n/g;

  return string.replace(cr, '\\r').replace(lf, '\\n');
}

function string2arraybuffer(str) {
  var buf = new ArrayBuffer(str.length);
  var bufView = new Uint8Array(buf);
  for (var i=0, strLen=str.length; i<strLen; i++) {
    bufView[i] = str.charCodeAt(i);
  }
  return bufView;
}

function is_intel_hex(str) {
  const intelHex = /^:[0-9a-fA-F][0-9a-fA-F]/;

  return str.match(intelHex);
}

function hex_to_bin (intel_hex_data) {
    return intel_hex.parse(intel_hex_data).data;
}

async function delay(milliseconds) {
    await new Promise(resolve => {setTimeout(() => {resolve()}, milliseconds)});
}

function flush() {
  stdin_buffer = '';
}

function readline(terminator, tries) {
  return new Promise((resolve, reject) => {
    if (typeof tries === 'undefined') {
      tries = 10;
    }

    function check() {
      let pos = stdin_buffer.search(terminator);
      if (pos >= 0) {
        pos += terminator.length;
        const line = stdin_buffer.substring(0, pos);
        stdin_buffer = stdin_buffer.substring(pos);
        resolve(line);
        return;
      }

      tries -= 1;
      if (tries <= 0) {
        reject("Timeout in readline()");
        return;
      }
      setTimeout(check, 100);
    }
    check();
  });
}

async function expect(expected_string) {
  const answer = await readline('\r\n');
  if (answer != expected_string) {
    throw('Expected "' + make_crlf_visible(expected_string) + '", got "' + make_crlf_visible(answer));
  }
  return answer;
}