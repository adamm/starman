body_w = 28;
body_l = 18;
body_h = 100;
body_x = -(body_w / 2);
body_y = -(body_l / 2);
body_z = 0;

// How far below the top of the body will the socket and treebranch reach?
body_socket_delta = 12;
body_treebranch_delta = 10;

socket_w = 3;
socket_l = 22;
socket_h = 31;

socket_left_x = -8.5 - socket_w;
socket_left_y = -(socket_l / 2);
socket_left_z = body_h - socket_h - body_socket_delta;

socket_right_x = 8.5;
socket_right_y = -(socket_l / 2);
socket_right_z = body_h - socket_h - body_socket_delta;

tree_cylinder_x = 0;
tree_cylinder_y = 0;
tree_cylinder_z = body_h - 50;
tree_cylinder_r = 7;
tree_cylinder_h = 50;

tree_cone_x = 0;
tree_cone_y = 0;
tree_cone_z = -0.01;
tree_cone_r = 7;
tree_cone_h = 60;

$fn = 100;

module mount_3d() {

    difference() {
        union() {
            translate([body_x, body_y, body_z])
                cube([body_w, body_l, body_h]);

            translate([tree_cone_x, tree_cone_y, tree_cone_z])
                cylinder(r=tree_cone_r, h=tree_cone_h, d1=40);
        }

        translate([socket_left_x, socket_left_y, socket_left_z])
            cube([socket_w, socket_l, socket_h]);

        translate([socket_right_x, socket_right_y, socket_right_z])
            cube([socket_w, socket_l, socket_h]);

        translate([tree_cylinder_x, tree_cylinder_y, tree_cylinder_z-2])
            cylinder(r=tree_cylinder_r, h=tree_cylinder_h);

        translate([tree_cone_x, tree_cone_y, tree_cone_z-2])
            cylinder(r=tree_cone_r, h=tree_cone_h, d1=35);
    }
}


mount_3d();

// vim:syntax=c
