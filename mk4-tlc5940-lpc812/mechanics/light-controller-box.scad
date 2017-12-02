// Simple box to put the Light Controller MK4 inside.

$fn = 50;
eps = 0.05;
eps2 = 2 * eps;
epsz = [0, 0, -eps];

dim_inside = [55, 25, 6];
wall_t = 1;
bottom_t = 0.6;
top_t = 0.6;
hole_d = 5;

dim_outside = dim_inside + [2*wall_t, 2*wall_t, bottom_t];
dim_lid = [dim_outside.x, dim_outside.y, top_t];

difference() {
    cube(dim_outside);
    translate([wall_t, wall_t, bottom_t])
        cube(dim_inside + [0, 0, eps2]);
    translate([wall_t+eps, dim_outside.y/2, dim_outside.z-hole_d/2]) rotate([0, -90, 0])  {
        cylinder(d=hole_d, h=wall_t+eps2);
        translate([0, -hole_d/2, 0])  cube([hole_d/2+eps, hole_d, wall_t+eps2]);
    }
}

translate([0, dim_outside.y+3, 0])
cube(dim_lid);