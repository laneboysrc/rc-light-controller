// Case for the LANE Boys RC DIY light controller
//
// Print with 0.2 mm layer height

$fn = 50;

h_bottom = 0.6;
h_middle = 1.2;

rotate([180, 0, 0]) {
    linear_extrude(h_middle)
        import("middle.dxf");
    translate([0, 0, h_middle]) linear_extrude(h_bottom)
        import("bottom.dxf");
}