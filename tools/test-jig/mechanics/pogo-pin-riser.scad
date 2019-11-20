
$fa = 1;
$fs = 0.2;

fudge = 0.05;
fudge2 = 2 * fudge;


pcb_t = 1.6;

pogo_pin_d = 1;
pogo_pin_l = 16.5;

riser = [28, 18, 15-pcb_t];
riser_pos = [-1.78, -1.78, 0];

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
    translate(riser_pos) cube(riser);
    pogo_pins();
    mounting_holes();
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
    d_solder = 5;
    h_solder = pcb_t * 3;
    
    cylinder(h=pogo_pin_l, d=d);
    cylinder(h=h_solder, d=d_solder);
    
}
    
