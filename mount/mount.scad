body_w = 28;
body_l = 18;
body_h = 80;
body_x = -(body_w / 2);
body_y = -(body_l / 2);
body_z = 0;

// How far below the top of the body will the socket and treebranch reach?
body_socket_delta = 12;
body_treebranch_delta = 5;

socket_w = 3;
socket_l = 22;
socket_h = 31;

socket_left_x = -8.5 - socket_w;
socket_left_y = -(socket_l / 2);
socket_left_z = body_h - socket_h - body_socket_delta;

socket_right_x = 8.5;
socket_right_y = -(socket_l / 2);
socket_right_z = body_h - socket_h - body_socket_delta;

tree_branch_x = 0;
tree_branch_y = 0;
tree_branch_z = -1;
tree_branch_r = 7;
tree_branch_h = body_h - body_treebranch_delta;

$fn = 100;

module mount_3d() {
    difference() {
        translate([body_x, body_y, body_z])
            cube([body_w, body_l, body_h]);

        translate([socket_left_x, socket_left_y, socket_left_z])
            cube([socket_w, socket_l, socket_h]);

        translate([socket_right_x, socket_right_y, socket_right_z])
            cube([socket_w, socket_l, socket_h]);

        translate([tree_branch_x, tree_branch_y, tree_branch_z])
            cylinder(r=tree_branch_r, h=tree_branch_h);
    }
}


mount_3d();

// vim:syntax=c
