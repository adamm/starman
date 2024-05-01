include <pcb.scad>;


board_w = 280;
board_h = 3.0;
board_l = 250;
board_x = -50;
board_y = -50;
board_z = 0.01;

stencil_w = 240;
stencil_h = 3;
stencil_l = 210;
stencil_x = -30;
stencil_y = -30;
stencil_z = 3;

module negative_pcb(x, y, z) {
    translate([x, y, z+1.4])
        pcb(false);
    translate([x, y, z])
        linear_extrude(height=board_h+4)
            offset(delta=-2.5)
                projection(cut=true)
                    pcb(false);
}

module board() {
    difference() {
        translate([board_x, board_y, board_z])
            cube([board_w, board_l, board_h]);
        //translate([stencil_x, stencil_y, stencil_z])
            //cube([stencil_w, stencil_l, stencil_h]);
        negative_pcb(0, 0, 0);
    }
}

board();
