"use strict";
/*jslint node: true*/

var gulp = require("gulp");
var inlinesource = require('gulp-inline-source');
var template = require('gulp-j140');
var rename = require("gulp-rename");

function get_date() {
    var date;

    function pad(number) {
        if (number < 10) {
            return '0' + number;
        }
        return number;
    }

    date = new Date();
    date = date.getUTCFullYear() + '-' + pad(date.getUTCMonth() + 1) + '-' +
        pad(date.getUTCDate());

    return date;
}


gulp.task('default', function () {
    var options = {
        compress: true,
        swallowErrors: false,
    };

    return gulp.src('./source.html')
        .pipe(template({date: get_date()}))
        .pipe(inlinesource(options)).on('error', errorHandler)
        .pipe(rename('configurator.html'))
        .pipe(gulp.dest("./build"));
});


gulp.task('no-compress', function () {
    var options = {
        compress: false,
        swallowErrors: false,
    };

    return gulp.src('./source.html')
        .pipe(inlinesource(options)).on('error', errorHandler)
        .pipe(rename('configurator.html'))
        .pipe(gulp.dest("./build"));
});


function errorHandler (error) {
  console.log(error.toString());
  console.log('');
  console.log('!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!');
  console.log('In-lining of Javascript or CSS failed!');
  console.log('');
  console.log('Have you built the Light Programm assembler? Change to the "assembler/"');
  console.log('directory and run "make".');
  console.log('!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!');
  console.log('');
  this.emit('exit');
}