"use strict";
/*jslint node: true*/

var gulp = require("gulp");
var inlinesource = require('gulp-inline-source');
var template = require('gulp-j140');

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
        swallowErrors: true,
    };

    return gulp.src('./configurator.html')
        .pipe(template({date: get_date()}))
        .pipe(inlinesource(options))
        .pipe(gulp.dest("./build"));
});


gulp.task('no-compress', function () {
    var options = {
        compress: false,
        swallowErrors: true,
    };

    return gulp.src('./configurator.html')
        .pipe(inlinesource(options))
        .pipe(gulp.dest("./build"));
});
