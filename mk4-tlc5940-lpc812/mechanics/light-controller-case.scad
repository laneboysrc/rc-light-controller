// Case for the LANE Boys RC DIY light controller
//
// Print with 0.2 mm layer height
//
// Modified on 2021-11-03 for compatibility with Mk4 Rev 6 (LPC832)

$fn = 50;
eps = 0.05;

h_bottom = 0.6;
h_middle = 1.5;

h_total = h_bottom + h_middle;

h_cutout = h_middle + eps;

difference() {
    translate([0, 0, 0]) linear_extrude(h_total-eps)
        import("bottom.dxf");

    translate([0, 0, h_bottom]) {
        translate([2, 5, 0]) cube([16, 8.2, h_cutout]);
        translate([8.4, 5, 0]) cube([16, 10.8, h_cutout]);
        translate([14, 1.8, 0]) cube([10, 5, h_cutout]);
        translate([22.5, -eps, 0]) cube([10, 18+eps+eps, h_cutout]);
    }
}