// Case for the LANE Boys RC DIY light controller 
//
// Print with 0.2 mm layer height
//
// Modified on 2022-05-12 for compatibility with Mk4 Rev 7 (LPC832, S.Bus)

$fn = 50;
eps = 0.05;

h_bottom = 0.6;
h_middle = 1.5;

h_total = h_bottom + h_middle;

h_cutout = h_middle + eps;

difference() {
    rounded_cube([29, 18, h_total-eps], r=1);

    translate([0, 0, h_bottom]) {
        translate([2, 5, 0]) cube([18, 8.2, h_cutout]);
        translate([8.4, 5, 0]) cube([18, 10.8, h_cutout]);
        translate([14, 1.8, 0]) cube([12, 5, h_cutout]);
        translate([24.5, -eps, 0]) cube([10, 18+eps+eps, h_cutout]);
    }
}

module rounded_cube(dim, r=3, center="") {
    d = 2 * r;
    fudge = 0.05;
    
    x = search("x", center) ? 0 : dim.x/2;
    y = search("y", center) ? 0 : dim.y/2;
    z = search("z", center) ? 0 : dim.z/2;
    translate([x, y, z])
        minkowski() {
            cube(dim - [d, d, fudge], true);
            cylinder(r=r, h=fudge);
        }
}