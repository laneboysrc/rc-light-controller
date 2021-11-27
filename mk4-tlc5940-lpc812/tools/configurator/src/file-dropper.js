'use strict';
/*

  This is a file dropper implementation that allows users to drage
  light controller configuration files (light_controller.config.txt)
  from the file manager onto the Configurator application.

*/


class File_dropper {
  // --------------------------------------------------------------------------
  constructor(file_dropped_callback) {
    this.callback = file_dropped_callback;

    if (typeof this.callback !== 'function') {
      throw('No callback function provided');
    }

    this.container = document.querySelector('.file-dropper-container');
    this.zone = document.querySelector('.file-dropper-zone');

    if (!this.container) {
      throw('Could not find required element with class file-dropper-container');
    }
    if (!this.zone) {
      throw('Could not find required element with class file-dropper-zone');
    }

    const html = document.querySelector('html');
    html.addEventListener('dragenter', this.dragEnterHandler);
    this.zone.addEventListener('dragover', this.dragOverHandler);
    this.zone.addEventListener('dragleave', this.dragLeaveHandler);
    this.zone.addEventListener('drop', this.dropHandler);
  }

  // --------------------------------------------------------------------------
  dragEnterHandler = (event) => {
    event.preventDefault();
    this.container.classList.remove('hidden');
  }

  // --------------------------------------------------------------------------
  dragOverHandler = (event) => {
    event.preventDefault();
  }

  // --------------------------------------------------------------------------
  dragLeaveHandler = (event) => {
    event.preventDefault();
    this.container.classList.add('hidden');
  }

  // --------------------------------------------------------------------------
  dropHandler = (event) => {
    event.preventDefault();

    this.container.classList.add('hidden');

    if (event.dataTransfer.items) {
      if (event.dataTransfer.items.length > 0) {
        const item = event.dataTransfer.items[0];
        if (item.kind === 'file') {
          const file = item.getAsFile();
          this.callback(file);
        }
      }
      else {
        console.warn('event.dataTransfer.items.length is 0 or negative?!');
      }
    } else {
      if (event.dataTransfer.files.length > 0) {
        const file = event.dataTransfer.files[0];
        this.callback(file);
      }
      else {
        console.warn('event.dataTransfer.files.length is 0 or negative?!');
      }
    }
  }
}

