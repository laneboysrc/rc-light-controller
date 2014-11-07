var gulp = require("gulp");
var inlinesource = require('gulp-inline-source');

gulp.task('default', function() {
    var options = {
        compress: true,
        swallowErrors: true,
    };

   return gulp.src('./configurator.html')
        .pipe(inlinesource(options))
        .pipe(gulp.dest("./build"));
});
