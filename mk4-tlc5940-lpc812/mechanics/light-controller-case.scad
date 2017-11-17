// Case for the LANE Boys RC DIY light controller
//
// Print with 0.2 mm layer height

$fn = 50;

h_bottom = 0.6;
h_middle = 1.2;

linear_extrude(h_bottom)
    import("bottom.dxf");
linear_extrude(h_bottom+h_middle)
    import("middle.dxf");