"use strict";
/*jslint node: true*/

var gulp = require("gulp");
var inlinesource = require('gulp-inline-source');


gulp.task('default', function () {
    var options = {
        compress: true,
        swallowErrors: true,
    };

    return gulp.src('./configurator.html')
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
