#!/usr/bin/env python3

import base64
import os
import re
import sys

def perform_inline(filename, regexp):
    with open(filename, 'rt') as input_file:
        for line in input_file:
            m = regexp.search(line)
            if m:
                matched_filename = m.group(0)

                if matched_filename.endswith('.css'):
                    print('<style>')
                    perform_inline(matched_filename, regexp)
                    print('\n</style>')
                elif matched_filename.endswith('.js'):
                    print('<script>')
                    perform_inline(matched_filename, regexp)
                    print('\n</script>')
                elif matched_filename.endswith('.jpg'):
                    with open(matched_filename, 'rb') as f:
                        font = f.read()
                        replacement = 'data:image/jpeg;base64,{}'.format(base64.b64encode(font).decode('ascii'))
                        print(regexp.sub(replacement, line), end='')
                elif matched_filename.endswith('.mp3'):
                    with open(matched_filename, 'rb') as f:
                        font = f.read()
                        replacement = 'data:audio/mp3;base64,{}'.format(base64.b64encode(font).decode('ascii'))
                        print(regexp.sub(replacement, line), end='')
                elif matched_filename.endswith('.ttf'):
                    with open(matched_filename, 'rb') as f:
                        font = f.read()
                        replacement = '"data:application/octet-stream;base64,{}"'.format(base64.b64encode(font).decode('ascii'))
                        print(regexp.sub(replacement, line), end='')
                else:
                    print('Do not know how to inline file {}'.format(matched_filename), file=sys.stderr)
                    print(line, end='')

            else:
                print(line, end='')


def run_command():
    if len(sys.argv) != 2:
        print('Usage: inline-media.py <input.html> >output.html')
        sys.exit(1)

    html_file = sys.argv[1]

    dir_content = os.listdir(os.path.dirname(os.path.abspath(html_file)))
    file_list = [f for f in dir_content if os.path.isfile(f)]

    regexp = re.compile( '|'.join(re.escape(f) for f in file_list))

    perform_inline(html_file, regexp)


if __name__ == '__main__':
    run_command();

