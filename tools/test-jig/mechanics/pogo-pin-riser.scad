
$fa = 1;
$fs = 0.2;

fudge = 0.05;
fudge2 = 2 * fudge;

// Light controller PCB dimension
pcb_dim = [27, 18, 0.8];

// Test jig PCB thickness
pcb_t = 1.6;

pogo_pin_d = 1;
pogo_pin_l = 16.5;
pogo_pin_head_d = 1.75;
pogo_pin_head_l = 7;

riser_dim = [pcb_dim.x, pcb_dim.y, 15-pcb_t];
riser_pos = [-1.78, -1.78, 0];

chute_w = 2;
wall_t = 1;

mounting_hole_d = 1.75;
mounting_hole_l = 9;
mounting_hole_pos = [
    [6, 7],
    [16, 7],
];


pin_pos = [
    [0, 0],         // OUT11       
    [2.8, 0],       // OUT12
    [4.8, 0],       // OUT13
    [6.9, 0],       // OUT14
    [8.9, 0],       // OUT15
    [10.9, 0],      // OUT15S
    [14.7, 0],      // +LED1 
    //[16.8, 0],   // +LED2

    [0, 2],         // OUT10
    [0, 4.1],       // OUT9
    [0, 6.1],       // OUT8
    [0, 8.4],       // OUT7
    [0, 10.4],      // OUT6
    [0, 12.4],      // OUT5
    [0, 14.4],      // OUT4

    [2.8, 14.4],    // OUT3
    [4.8, 14.4],    // OUT2
    [6.9, 14.4],    // OUT1
    [8.9, 14.4],    // OUT0
];


difference() {
    riser();
    pogo_pins();
    mounting_holes();
}

module riser() {
    translate(riser_pos) {
        
        cube(riser_dim);
        translate([0, 0, riser_dim.z]) chute();
    }

}



module mounting_holes() {
    for (pos = mounting_hole_pos) {
        translate([pos.x, pos.y, -fudge]) mounting_hole();
    }
}

module mounting_hole() {
    cylinder(h=mounting_hole_l, d=mounting_hole_d);
}

module pogo_pins() {
    for (pos = pin_pos) {
        translate([pos.x, pos.y, -pcb_t]) pogo_pin_clearance();
    }
}


module pogo_pin_clearance() {
    d = pogo_pin_d + 0.5;
    d_solder = 6;
    h_solder = pcb_t * 3;
    d_head = pogo_pin_head_d + 0.5;
    
    cylinder(h=pogo_pin_l, d=d);
    cylinder(h=h_solder, d1=d_solder, d2=d);
    translate([0, 0, pogo_pin_l-pogo_pin_head_l]) cylinder(h=pogo_pin_head_l, d=d_head);
    
}
    
module chute() {
    inner_bottom = [pcb_dim.x, pcb_dim.y];
    inner_top = [pcb_dim.x+chute_w, pcb_dim.y+chute_w];
    
    outer_bottom = inner_bottom + [2*wall_t, 2*wall_t, 0];
    outer_top = inner_top + [2*wall_t, 2*wall_t, 0];
    
    difference() {
        translate([-wall_t, -wall_t, 0]) hull() {
            linear_extrude(fudge2) square(outer_bottom);
            translate([-chute_w/2, -chute_w/2, chute_w]) linear_extrude(fudge) square(outer_top);
        }
        
        translate([0, 0, -fudge])hull() {
            linear_extrude(fudge2) square(inner_bottom);
            translate([-chute_w/2, -chute_w/2, chute_w+fudge2]) linear_extrude(fudge) square(inner_top);
        }
    }
    
    
}
