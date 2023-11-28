// Dimensions of the box
length = 10;  // in cm
width = 5;  // in cm
height = 8.5;  // in cm

// Thickness of the box walls
wall_thickness = 0.3;  // in cm

// Fillet radius for rounded edges
fillet_radius = 0.3;  // in cm

// Hole dimensions
hole_diameter = 1.5;  // diameter of the holes in the bottom and side

// Create the main box
difference() {
    // Outer box
    translate([0, 0, -fillet_radius])
        roundedCube([length, width, height + 2*fillet_radius], fillet_radius);

    // Subtract the inner box to create walls
    translate([wall_thickness, wall_thickness, wall_thickness])
        roundedCube([length - 2*wall_thickness, width - 2*wall_thickness, height], fillet_radius);

    // Holes for Adafruit_SSD1306
    translate([length/2-3, width/2-0.5, 0])
        rotate([90, 0, 0])
        cylinder(h = 2, d = 3, center = true);

    // Holes for knobs
    translate([length/2+4, width/2+1, 0])
        rotate([0, 0, 90])
        cylinder(h = 2*wall_thickness, d = hole_diameter, center = true, $fn=25);
    translate([length/2+4, width/2-1, 0])
        rotate([0, 0, 90])
        cylinder(h = 2*wall_thickness, d = hole_diameter, center = true, $fn=25);

    // Holes for speakers on the sides
    translate([0, width/2, height/4])
        rotate([0, 90, 0])
        cylinder(h = height/2, d = hole_diameter, center = true);
    translate([length, width/2, height/4])
        rotate([0, 90, 0])
        cylinder(h = height/2, d = hole_diameter, center = true);
}

// Define a rounded cube using cylinders
module roundedCube(size, r) {
    hull() {
        translate([r, r, 0])
            cylinder(h = size[2], d = r*2, $fn = 100);
        translate([r, size[1] - r, 0])
            cylinder(h = size[2], d = r*2, $fn = 100);
        translate([size[0] - r, r, 0])
            cylinder(h = size[2], d = r*2, $fn = 100);
        translate([size[0] - r, size[1] - r, 0])
            cylinder(h = size[2], d = r*2, $fn = 100);
        translate([0, 0, r])
            cylinder(h = size[2], d = r*2, $fn = 100);
        translate([0, size[1], r])
            cylinder(h = size[2], d = r*2, $fn = 100);
        translate([size[0], 0, r])
            cylinder(h = size[2], d = r*2, $fn = 100);
        translate([size[0], size[1], r])
            cylinder(h = size[2], d = r*2, $fn = 100);
    }
}
