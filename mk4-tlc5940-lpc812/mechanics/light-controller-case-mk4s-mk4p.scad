// Case for the LANE Boys RC DIY light controller Mk4S and Mk4P
//
// Print with 0.2 mm layer height

$fn = 50;
fudge = 0.05;
fudge2 = 2 * fudge;

gap = 0.3;

wall_t = 1.5;
bottom_t = 1;

pcb = [44, 23, 1.6];

cutout_z = 3.5;


pcb_cutout = [pcb.x+2*gap, pcb.y+2*gap, cutout_z+fudge];
pcb_cutout_offset = [-gap, -gap, pcb.z-cutout_z];

dim = pcb_cutout + [2*wall_t, 2*wall_t, bottom_t-fudge];
case_offset = [-wall_t-gap, -wall_t-gap, pcb_cutout_offset.z-bottom_t];

connector_w = 16;

connector_cutout = [2*pcb.x, connector_w+2*gap, cutout_z+fudge];
connector_cutout_offset = pcb_cutout_offset-[pcb.x/2, 0, 0];

post_d1 = 4;
post_d12 = 6;
post_h1 = cutout_z-pcb.z - 0.3; // Reduce height slightly due to printing not being precise
hole_d = 1.7;                   // Determined through test prints, may not work on your printer...

post1_pos = [9, 5, pcb_cutout_offset.z-fudge];
post2_pos = [pcb.x-9, 5, pcb_cutout_offset.z-fudge];

hole1_pos = post1_pos - [0, 0, bottom_t-fudge-0.3];
hole2_pos = post2_pos - [0, 0, bottom_t-fudge-0.3];

difference() {
    union() {
        difference() {
            translate(case_offset) cube(dim);
            translate(pcb_cutout_offset) cube(pcb_cutout);
            translate(connector_cutout_offset) cube(connector_cutout);
        }

        translate(post1_pos) post();
        translate(post2_pos) post();
    }
    
    translate(hole1_pos) cylinder(d=hole_d, h=2*post_h1);
    translate(hole2_pos) cylinder(d=hole_d, h=2*post_h1);
}

module post() {
    cylinder(d1=post_d12, d2=post_d1, h=post_h1);
}

//color("salmon") cube(pcb);