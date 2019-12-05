
$fa = 1;
$fs = 0.2;

fudge = 0.05;
fudge2 = 2 * fudge;
clearance = 0.3;

// Light controller PCB dimension
pcb_dim = [27, 18, 0.8];

// Test jig PCB thickness
pcb_t = 1.6;

pogo_pin_d = 1;
pogo_pin_l = 16.5;
pogo_pin_head_d = 1.75;
pogo_pin_head_l = 7;

chute_w = 2;
wall_t = 1;

riser_dim = [pcb_dim.x+wall_t, pcb_dim.y+2*wall_t, 15-pcb_t];
riser_pos = [-1.78-wall_t, -1.78-wall_t, 0];

mounting_hole_d = 1.75;
mounting_hole_l = 9;
mounting_hole_pos = [
    [6, 7],
    [16, 7],
];

// Chute dimensions
inner_bottom = [pcb_dim.x, pcb_dim.y] - [fudge, fudge2];
inner_top = [pcb_dim.x+chute_w, pcb_dim.y+chute_w];
outer_bottom = inner_bottom;
outer_top = inner_top + [0, 2*wall_t, 0];


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


pogo_pin_riser();
//mirror([0, 0, 1]) clip();

module clip() {
    clip_dim = [10, outer_top.y+2*wall_t, chute_w+1+wall_t];

    module cavity() {
        x = clip_dim.x + fudge2;
        y1 = outer_bottom.y + clearance;
        y2 = outer_top.y + clearance;
        z1 = chute_w;
        z2 = chute_w + 1 + clearance;
        y1p = (y2-y1)/2;
        y2p = 0;
        
        translate([-fudge, wall_t, 0]) rotate([90, 0, 90]) linear_extrude(x) polygon([
            [y1p, -fudge],
            [y2p, z1],            
            [y2p, z2],            
            [y2p+y2, z2],            
            [y2p+y2, z1],           
            [y1p+y1, -fudge], 
        ]);
    }

    translate([0, -3*wall_t, 0]) difference() {
        cube(clip_dim);
        cavity();
    }
}


module pogo_pin_riser() {
    difference() {
        riser();
        pogo_pins();
        pogo_pins_head_clearance();
        mounting_holes();
        solder_clearance();
    }
}

module riser() {
    translate(riser_pos) {
        cube(riser_dim);
        translate([fudge+wall_t, fudge+wall_t, riser_dim.z]) chute();
//        translate([fudge+wall_t, fudge+wall_t, riser_dim.z]) clip();
    }
}

module solder_clearance() {
    w = 3.5;
    h1 = 2;
    h2 = 7;
    
    module shape(length=10) {
        translate([-fudge, -fudge, -fudge]) rotate([0, 90, 0]) linear_extrude(length+fudge2) polygon([
            [0, 0],
            [0, w],
            [-h1, w],
            [-h2, 0],
        ]);
    }
    
    translate(riser_pos) {
        shape(19);
        translate([0, riser_dim.y, 0]) mirror([0, 1, 0]) shape(13);
        mirror([0, 1, 0] ) rotate([0, 0, -90]) shape(riser_dim.y);
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

module pogo_pins_head_clearance() {
    h = pogo_pin_l-pogo_pin_head_l;
    d_head = pogo_pin_head_d + 1.5;
    
    translate([0, 0, h-pcb_t]) {
        hull() {
            translate(pin_pos[0]) cylinder(h=pogo_pin_head_l, d=d_head);
            translate(pin_pos[6]) cylinder(h=pogo_pin_head_l, d=d_head);
        }

        hull() {
            translate(pin_pos[0]) cylinder(h=pogo_pin_head_l, d=d_head);
            translate(pin_pos[13]) cylinder(h=pogo_pin_head_l, d=d_head);
        }
        
        hull() {
            translate(pin_pos[13]) cylinder(h=pogo_pin_head_l, d=d_head);
            translate(pin_pos[17]) cylinder(h=pogo_pin_head_l, d=d_head);
        }
    }
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
    d_head = pogo_pin_head_d + 1.5;
    
    cylinder(h=pogo_pin_l, d=d);
//    cylinder(h=h_solder, d1=d_solder, d2=d);
    translate([0, 0, pogo_pin_l-pogo_pin_head_l]) cylinder(h=pogo_pin_head_l, d=d_head);
}
    
module chute() {
     difference() {
        translate([-wall_t, -wall_t, 0]) hull() {
            translate([wall_t, wall_t, -chute_w]) linear_extrude(fudge2) square(outer_bottom);
            translate([-chute_w/2, -chute_w/2, chute_w]) linear_extrude(fudge) square(outer_top);
        }
        
        translate([0, 0, -fudge])hull() {
            linear_extrude(fudge2) square(inner_bottom);
            translate([-chute_w/2, -chute_w/2, chute_w+fudge2]) linear_extrude(fudge) square(inner_top);
        }
    }
}
