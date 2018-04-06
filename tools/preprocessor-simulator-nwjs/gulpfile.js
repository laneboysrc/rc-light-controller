//  gulp <task> [--platform ...]
//
//  gulp debug
//  gulp release
//  gulp release --linux32 --linux64 --win32 --win64
//  gulp release --linux32 --linux64 --win32 --win64 --osx64
//

'use strict';

const pkg = require('./package.json');

const os = require('os');
const fs = require('fs');
const path = require('path');
const child_process = require('child_process');

const del = require('del');
const gulp = require('gulp');
const rename = require('gulp-rename');
const zip = require('gulp-zip');

const NwBuilder = require('nw-builder');
const makensis = require('makensis');
const deb = require('gulp-debian');
const buildRpm = require('rpm-builder');
const commandExistsSync = require('command-exists').sync;


const APP_NAME = 'Preprocessor Simulator';
const APP_FILENAME = 'preprocessor-simulator';
const DIST_DIR = './build/dist/';
const DEBUG_DIR = './build/debug/';
const RELEASE_DIR = './build/release/';

const SUPPORTED_PLATFORMS = ['linux64', 'linux32', 'osx64', 'win32', 'win64'];

var nwBuilderOptions = {
    version: '0.29.3',
    files: DIST_DIR + '**/*',
    // winIco: './images/laneboysrc.ico',
    // macIcns: './images/laneboysrc.icns',
    // macPlist: { 'CFBundleDisplayName': 'Preprocessor Simulator'}
};


// ****************************************************************************
// Initializations before executing tasks
// ****************************************************************************
var platforms = get_build_platforms();


// ****************************************************************************
// Tasks
// ****************************************************************************
var clean = gulp.parallel(clean_dist, clean_debug, clean_release);
var distBuild = gulp.series(clean_dist, dist);
var debugBuild = gulp.series(gulp.parallel(clean_debug, distBuild), debug, run_debug_session);
var releaseBuild = gulp.series(gulp.parallel(clean_release, distBuild), release, gulp.parallel(get_packaging_tasks()));

gulp.task('default', debugBuild);
gulp.task('debug', debugBuild);
gulp.task('release', releaseBuild);
gulp.task('clean', clean);
gulp.task('clean-cache', clean_cache);


// ****************************************************************************
// Task functions
// ****************************************************************************
function clean_dist() {
    return del([DIST_DIR + '**'], { force: true });
}

function clean_debug() {
    return del([DEBUG_DIR + '**'], { force: true });
}

function clean_release() {
    return del([RELEASE_DIR + '**'], { force: true });
}

function clean_cache() {
    return del(['./cache/**'], { force: true });
}

function dist() {
    return gulp.src('preprocessor-simulator.html')
               .pipe(gulp.src('package.json', { passthrougth: true }))
               .pipe(gulp.src('manifest.json', { passthrougth: true }))
               .pipe(gulp.dest(DIST_DIR));
}

function release(done) {
    build_nw(platforms, RELEASE_DIR, 'normal', done);
}

function debug(done) {
    build_nw(platforms, DEBUG_DIR, 'sdk', done);
}

function run_debug_session(done) {
    if (platforms.length !== 1) {
        console.log('More than one platform specified, not starting debug session');
        done();
        return;
    }

    var platform = platforms[0];

    var command = '';
    var ext = '';

    if (platform === 'osx64') {
        command = 'open ';
        ext = '.app';
    }
    else if (platform.startsWith('win')) {
        ext = '.exe';
    }

    var run = command + path.join(DEBUG_DIR, pkg.name, platforms[0], pkg.name + ext);

    console.log('Starting app for debugging, press F12 to launch debug tools.');
    child_process.exec(run);
    done();
}

function build_nw(platforms, dir, type, done) {
    var buildOptions = {
        buildDir: dir,
        platforms: platforms,
        flavor: type
    };

    function callback(error) {
        if (error) {
            console.log('Error building NW apps: ' + error);
            process.exit(1);
        }
        done();
    }

    var aggregatedOptions = Object.assign({}, nwBuilderOptions, buildOptions);

    var builder = new NwBuilder(aggregatedOptions);
    builder.on('log', console.log);
    builder.build(callback);
}

// ****************************************************************************
// Packaging functions
// ****************************************************************************
function package_linux64_zip(done) {
    return package_zip('linux64', done);
}

function package_linux64_deb(done) {
    return package_deb('linux64', done);
}

function package_linux64_rpm(done) {
    return package_rpm('linux64', done);
}

function package_linux32_zip(done) {
    return package_zip('linux32', done);
}

function package_linux32_deb(done) {
    return package_deb('linux32', done);
}

function package_linux32_rpm(done) {
    return package_rpm('linux32', done);
}

function package_win32(done) {
    return package_windows_installer('win32', done);
}

function package_win64(done) {
    return package_windows_installer('win64', done);
}

function get_package_arch(type, arch) {
    switch (arch) {
    case 'linux32':
        return 'i386';

    case 'linux64':
        if (type == 'rpm') {
            return 'x86_64';
        }
        return 'amd64';

    default:
        console.error(`Unsupported arch ${arch} for ${type}`);
        process.exit(1);
    }
}

function get_package_filename(platform, ext) {
    return APP_FILENAME + '_' + pkg.version + '_' + platform + '.' + ext;
}

function package_deb(arch, done) {
    if (!commandExistsSync('dpkg-deb')) {
        console.warn('dpkg-deb command not found, not generating deb package for ' + arch);
        return done();
    }

    return gulp.src([path.join(RELEASE_DIR, pkg.name, arch, '*')])
        .pipe(deb({
            package: pkg.name,
            version: pkg.version,
            maintainer: pkg.author,
            description: pkg.description,
            architecture: get_package_arch('deb', arch),
            section: 'base',
            priority: 'optional',
            depends: 'libgconf-2-4',
            postinst: [`xdg-desktop-menu install /opt/rc-light-controller/${APP_FILENAME}/${APP_FILENAME}.desktop`],
            prerm: [`xdg-desktop-menu uninstall ${APP_FILENAME}.desktop`],
            changelog: [],
            _target: `/opt/rc-light-controller/${APP_FILENAME}`,
            _out: RELEASE_DIR,
            _copyright: 'LICENSE',
            _clean: true
        }));
}

function package_rpm(arch, done) {
    mkdir_p(RELEASE_DIR);

    var options = {
        name: pkg.name,
        version: pkg.version,
        vendor: pkg.author,
        summary: pkg.description,
        buildArch: get_package_arch('rpm', arch),
        license: 'MIT',
        requires: 'libgconf-2-4',
        prefix: '/opt',
        files:
             [ { cwd: path.join(RELEASE_DIR, pkg.name, arch),
                 src: '*',
                 dest: `/opt/rc-light-controller/${APP_FILENAME}` } ],
        postInstallScript: [`xdg-desktop-menu install /opt/rc-light-controller/${APP_FILENAME}/${APP_FILENAME}.desktop`],
        preUninstallScript: [`xdg-desktop-menu uninstall ${APP_FILENAME}.desktop`],
        tempDir: path.join(RELEASE_DIR,'tmp-rpm-build-' + arch),
        keepTemp: false,
        verbose: false,
        rpmDest: RELEASE_DIR
    };

    buildRpm(options, function(error) {
        if (error) {
            console.error(`Failed to generate rpm package for ${arch}: ${error}`);
        }
        done();
    });
}

function package_zip(arch) {
    var basePath = path.join(RELEASE_DIR, pkg.name, arch);
    var srcPath = path.join(basePath, '**');
    var outputFile = get_package_filename(arch, 'zip');

    function prepend_app_path (p) {
        p.dirname = path.join(APP_FILENAME, p.dirname);
    }

    return gulp.src(srcPath, { base: basePath })
               .pipe(rename(prepend_app_path))
               .pipe(zip(outputFile))
               .pipe(gulp.dest(RELEASE_DIR));
}

function package_windows_installer(arch, done) {
    const options = {
        verbose: 2,
        define: {
            'VERSION': pkg.version,
            'PLATFORM': arch,
            'DEST_FOLDER': RELEASE_DIR
        }
    };

    if (!commandExistsSync('makensis')) {
        console.warn(`makensis not found, not generating installer for ${arch}. Please install with "sudo apt install nsis"`);
        return done();
    }

    mkdir_p(RELEASE_DIR);

    var output = makensis.compileSync('./assets/windows/installer.nsi', options);
    if (output.status !== 0) {
        console.error(`makensis for platform ${arch} finished with error ${output.status}: ${output.stderr}`);
    }

    done();
}

function package_osx() {
    var appdmg = require('gulp-appdmg');

    mkdir_p(RELEASE_DIR);

    return gulp.src(['.'])
        .pipe(appdmg({
            target: path.join(RELEASE_DIR, get_package_filename('macOS', 'dmg')),
            basepath: path.join(RELEASE_DIR, pkg.name, 'osx64'),
            specification: {
                title: APP_NAME,
                contents: [
                    { 'x': 448, 'y': 342, 'type': 'link', 'path': '/Applications' },
                    { 'x': 192, 'y': 344, 'type': 'file', 'path': pkg.name + '.app', 'name': APP_NAME + '.app' }
                ],
                // background: path.join(__dirname, 'images/dmg-background.png'),
                format: 'UDZO',
                window: {
                    size: {
                        width: 638,
                        height: 479
                    }
                }
            },
        })
    );
}


// ****************************************************************************
// Utilities
// ****************************************************************************
function get_packaging_tasks() {
    var tasks = [];

    if (is_platform_requested('linux64')) {
        tasks.push(package_linux64_deb);
        tasks.push(package_linux64_rpm);
        tasks.push(package_linux64_zip);
    }

    if (is_platform_requested('linux32')) {
        tasks.push(package_linux32_deb);
        tasks.push(package_linux32_rpm);
        tasks.push(package_linux32_zip);
    }

    if (is_platform_requested('win64')) {
        tasks.push(package_win64);
    }

    if (is_platform_requested('win32')) {
        tasks.push(package_win32);
    }

    if (is_platform_requested('osx64')) {
        tasks.push(package_osx);
    }

    return tasks;
}

function get_build_platforms() {
    var platforms = [];

    for (var i = 3; i < process.argv.length; i++) {
        var arg = process.argv[i].replace(/^--/, '');
        if (SUPPORTED_PLATFORMS.indexOf(arg) >= 0) {
            platforms.push(arg);
        }
        else {
            console.error(`Unknown platform: ${arg}. Supported platforms: ${SUPPORTED_PLATFORMS}`);
            process.exit();
        }
    }

    if (platforms.length === 0) {
        switch (os.platform()) {
        case 'darwin':
            platforms.push('osx64');
            break;

        case 'linux':
            platforms.push('linux64');
            break;

        case 'win32':
            platforms.push('win32');
            break;

        default:
            console.error(`Host platform ${os.platform()} is not supported. Supported platforms: ${SUPPORTED_PLATFORMS}`);
            process.exit(1);
        }
    }

    console.log(`Building for platform(s): ${platforms}`);
    return platforms;
}

function mkdir_p(dir) {
    fs.mkdir(dir, '0775', function(error) {
        if (error  &&  error.code !== 'EEXIST') {
            throw error;
        }
    });
}

function is_platform_requested(platform) {
    if (platforms.indexOf(platform) !== -1) {
        return true;
    }
    return false;
}