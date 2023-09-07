// Case for the LANE Boys RC DIY light controller
// Fits into the Killerbody light control box
//
// Print with 0.2 mm layer height

$fn = 50;
eps = 0.05;

h_bottom = 0.6 + 0.5;
h_middle = 1.3;

h_total = h_bottom + h_middle;

h_cutout = h_middle + eps;

difference() {
    union() {
        translate([0, 0, 0]) linear_extrude(h_total-eps)
            import("bottom.dxf");
    
        // Extend to the left to brace inside the Killerbody case
        translate([-7, 0, 0]) cube([7+2, 18, h_bottom]);
    }
    
    // Hole for feeding the wires 
    translate([-3, 10, -eps]) cylinder(d=4, h=h_total+eps+eps);
    
    translate([0, 0, h_bottom]) {
        translate([3, 5, 0]) cube([16, 8.2, h_cutout]);
        translate([8.4, 5, 0]) cube([16, 10.8, h_cutout]);
        translate([14.8, 4, 0]) cube([5, 5, h_cutout]);
        translate([17, 1.8, 0]) cube([8, 5, h_cutout]);
        translate([22.5, -eps, 0]) cube([10, 18+eps+eps, h_cutout]);
    }
}