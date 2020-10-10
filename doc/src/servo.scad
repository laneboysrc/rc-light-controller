// Fictional servo with arm for drawings
dim_l = [24, 12, 16];
dim_m = [34, dim_l.y, 2];
dim_u = [dim_l.x, dim_l.y, 4];

pos_l = [-6, -dim_l.y/2, 0];
pos_m = pos_l + [-(dim_m.x-dim_l.x)/2, 0, dim_l.z];
pos_u = pos_l + [0, 0, dim_l.z+dim_m.z];

dome_h = pos_u.z + dim_u.z + 3;
dome_d = dim_l.y;

spline_h = dome_h + 3;
spline_d = 8;

hole_d = 2;
hole_h = dome_h;
hole_x = 2;

servo_z = spline_h;

servo();
color("orange") rotate([0, 0, -45]) translate([0, 0, servo_z]) servo_arm();

module servo() {
    // Lower case
    translate(pos_l) cube(dim_l);
    
    // Mounting plate
    difference() {
        translate(pos_m) cube(dim_m);
        translate([pos_m.x+hole_x, 0, 0]) cylinder(d=hole_d, h=hole_h, $fn=45);
        translate([pos_m.x+dim_m.x-hole_x, 0, 0]) cylinder(d=hole_d, h=hole_h, $fn=45);
    }
    
    // Upper case
    translate(pos_u) cube(dim_u);
    
    // Large dome
    cylinder(d=dome_d, h=dome_h, $fn=45);
    
    // Spline
    cylinder(d=spline_d, h=spline_h, $fn=45);
}

module servo_arm() {
    h = 1.5;
    d1 = 8;
    d2 = 4;
    l = 12;

    hull() {
        cylinder(d=d1, h=h, $fn=45);
        translate([0, l, 0]) cylinder(d=d2, h=h, $fn=45);
    }

}