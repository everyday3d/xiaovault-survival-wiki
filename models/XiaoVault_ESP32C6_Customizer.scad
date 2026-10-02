// ==============================================================================
// XiaoVault ESP32-C6 Snap Enclosure — MakerWorld Parametric Customizer
// Compatible with MakerWorld Parametric Model Maker & OpenSCAD Customizer
// Designed by BoxerDawg 3D / Everyday3D
// Tested for 100% Zero-Support FDM Printing (Bambu Lab A1 / P1S / X1C / Any FDM)
// ==============================================================================

/* [Part Selection & Print Layout] */
// Choose which components to render
part_mode = "both_flat"; // [both_flat:Base + Lid (Print-Ready Flat on Bed), base_only:Base Shell Only, lid_only:Lid Shell Only, assembled:Assembled 3D View]
// Spacing between Base and Lid when printing flat (mm)
part_spacing = 6.0; // [4.0:1.0:15.0]

/* [Fit & Printer Calibration] */
// Snap-fit perimeter clearance (mm) - Increase if fit is too tight on your printer
fit_clearance = 0.20; // [0.10:0.05:0.40]
// Snap detent bead protrusion (mm) - Controls click firmness
detent_protrusion = 0.22; // [0.14:0.02:0.32]

/* [Lanyard / Keyring Tab] */
// Include rear-corner lanyard / paracord loop
enable_lanyard = true;
// Lanyard through-hole diameter (mm)
lanyard_hole_dia = 4.0; // [2.5:0.5:6.0]

/* [Lid Cutouts & Ventilation] */
// Ventilation & pin access style
lid_style = "gpio_slots"; // [gpio_slots:Full GPIO Slots + Reset/Boot/LED, minimal:Reset/Boot + LED Holes Only, solid:Completely Solid EDC Shell]
// Include front USB-C charging cable notch
enable_usb = true;

/* [Personalization / Embossing] */
// Enable custom text on lid
enable_text = true;
// Custom text (e.g. Amateur Radio Callsign, Initials, "XIAOVAULT", or "WIKI")
custom_text = "XIAOVAULT";
// Text style
text_style = "debossed"; // [debossed:Debossed (Engraved into Lid), embossed:Embossed (Raised 3D Letters)]
// Text orientation
text_rotation = 90; // [90:Vertical (Lengthwise), 0:Horizontal]
// Text size (mm)
text_size = 3.6; // [2.5:0.2:5.5]
// Text depth / height (mm)
text_depth = 0.4; // [0.2:0.1:0.8]
// Y Offset adjustment for text position (mm)
text_offset_y = 0.0; // [-6.0:0.5:6.0]
// X Offset adjustment for text position (mm)
text_offset_x = 0.0; // [-4.0:0.5:4.0]
// Font family
text_font = "Liberation Sans:style=Bold";

/* [Advanced / Mesh Quality] */
// Circle smoothness (higher = smoother curves, keep 32-48 for fast MakerWorld preview)
$fn = 40; // [24:8:64]

// ==============================================================================
// INTERNAL ENGINEERING CONSTANTS (Calibrated to Seeed XIAO ESP32-C6)
// ==============================================================================
outer_w = 21.0;
outer_l = 24.5;
corner_r = 2.0;
base_h = 5.0;
lid_h = 5.0;
total_h = base_h + lid_h; // 10.0 mm
floor_t = 1.2;
roof_t = 1.2;

inner_w = 18.2;
inner_l = 21.3;
standoff_h = 0.8; // PCB floor at Z = 2.0 mm

loop_cx = 7.0;
loop_cy = 13.0;
loop_r_out = 4.2;

usb_w = 9.8;
usb_base_z1 = 2.4;
usb_lid_z2 = 6.2;

lip_ext = 1.15;
det_z_center = 4.35;
det_dz = 0.45;
det_len = 1.80;
det_y_locs = [-9.65, 9.65];

// ------------------------------------------------------------------------------
// 2D PROFILE HELPER MODULES
// ------------------------------------------------------------------------------
module rounded_rect_2d(w, l, r) {
    hull() {
        translate([-w/2 + r, -l/2 + r]) circle(r=r);
        translate([ w/2 - r, -l/2 + r]) circle(r=r);
        translate([-w/2 + r,  l/2 - r]) circle(r=r);
        translate([ w/2 - r,  l/2 - r]) circle(r=r);
    }
}

module body_profile_2d() {
    union() {
        rounded_rect_2d(outer_w, outer_l, corner_r);
        if (enable_lanyard) {
            hull() {
                translate([loop_cx, loop_cy]) circle(r=loop_r_out);
                translate([loop_cx - 2.0, loop_cy - 3.5]) circle(r=2.0);
                translate([outer_w/2 - 2.0, outer_l/2 - 2.0]) circle(r=2.0);
            }
        }
    }
}

// ------------------------------------------------------------------------------
// BASE SHELL MODULE (Z = 0 to 5.0 mm)
// ------------------------------------------------------------------------------
module xiao_case_base() {
    difference() {
        // 1. Solid Outer Block
        linear_extrude(height=base_h)
            body_profile_2d();

        // 2. Lanyard Hole
        if (enable_lanyard) {
            translate([loop_cx, loop_cy, -1])
                cylinder(d=lanyard_hole_dia, h=base_h + 2);
        }

        // 3. Internal Board Cavity
        translate([-inner_w/2, -inner_l/2, floor_t])
            cube([inner_w, inner_l, base_h - floor_t + 1]);

        // 4. USB-C Notch
        if (enable_usb) {
            translate([-usb_w/2, -outer_l/2 - 1, usb_base_z1])
                cube([usb_w, 4.0, base_h - usb_base_z1 + 1]);
        }

        // 5. Coin / Fingernail Pry Notch
        translate([-outer_w/2 - 1, -2.0, base_h - 0.8])
            cube([2.0, 4.0, 1.2]);

        // 6. Female Snap Detent Grooves (4 positions: 2 left, 2 right)
        for (side = [-1, 1]) {
            for (dy = det_y_locs) {
                translate([side * (inner_w/2), dy, det_z_center])
                    rotate([0, side * 90, 0])
                        rotate([0, 0, 90])
                            hull() {
                                translate([-det_len/2, 0, 0])
                                    cylinder(r1=0, r2=detent_protrusion + fit_clearance/2 + 0.05, h=det_dz, center=false);
                                translate([det_len/2, 0, 0])
                                    cylinder(r1=0, r2=detent_protrusion + fit_clearance/2 + 0.05, h=det_dz, center=false);
                                translate([-det_len/2, 0, -det_dz])
                                    cylinder(r1=detent_protrusion + fit_clearance/2 + 0.05, r2=0, h=det_dz, center=false);
                                translate([det_len/2, 0, -det_dz])
                                    cylinder(r1=detent_protrusion + fit_clearance/2 + 0.05, r2=0, h=det_dz, center=false);
                            }
            }
        }
    }

    // 7. Corner Standoff Ledges supporting XIAO PCB at Z = 2.0 mm
    for (sx = [-inner_w/2, inner_w/2 - 2.2]) {
        for (sy = [-inner_l/2, inner_l/2 - 2.2]) {
            translate([sx, sy, floor_t])
                cube([2.2, 2.2, standoff_h]);
        }
    }
}

// ------------------------------------------------------------------------------
// LID SHELL MODULE (Assembled at Z = 5.0 to 10.0 mm)
// ------------------------------------------------------------------------------
module xiao_case_lid_raw() {
    lip_w = inner_w - 2 * fit_clearance;
    lip_l = inner_l - 2 * fit_clearance;
    cav_w = lip_w - 2 * 1.0;
    cav_l = lip_l - 2 * 1.0;

    difference() {
        union() {
            // Main Outer Shell (Z = 5.0 to 10.0)
            translate([0, 0, base_h])
                linear_extrude(height=lid_h)
                    body_profile_2d();

            // Male Alignment Lip extending down into Base (Z = 5.0 - lip_ext to 5.0)
            translate([-lip_w/2, -lip_l/2, base_h - lip_ext])
                cube([lip_w, lip_l, lip_ext + 0.1]);

            // Male Snap Detent Beads on Lip
            for (side = [-1, 1]) {
                for (dy = det_y_locs) {
                    translate([side * (lip_w/2), dy, det_z_center])
                        rotate([0, side * 90, 0])
                            rotate([0, 0, 90])
                                hull() {
                                    translate([-det_len/2, 0, 0])
                                        cylinder(r1=0, r2=detent_protrusion, h=det_dz, center=false);
                                    translate([det_len/2, 0, 0])
                                        cylinder(r1=0, r2=detent_protrusion, h=det_dz, center=false);
                                    translate([-det_len/2, 0, -det_dz])
                                        cylinder(r1=detent_protrusion, r2=0, h=det_dz, center=false);
                                    translate([det_len/2, 0, -det_dz])
                                        cylinder(r1=detent_protrusion, r2=0, h=det_dz, center=false);
                                }
                }
            }
        }

        // Lanyard Through-Hole
        if (enable_lanyard) {
            translate([loop_cx, loop_cy, base_h - 2])
                cylinder(d=lanyard_hole_dia, h=lid_h + 4);
        }

        // Internal Headroom Cavity
        translate([-cav_w/2, -cav_l/2, base_h - lip_ext - 1])
            cube([cav_w, cav_l, (total_h - roof_t) - (base_h - lip_ext) + 1]);

        // USB-C Top Split Notch
        if (enable_usb) {
            translate([-usb_w/2, -outer_l/2 - 1, base_h - 0.1])
                cube([usb_w, 4.0, usb_lid_z2 - base_h + 0.1]);
        }

        // Pry Notch matching Base
        translate([-outer_w/2 - 1, -2.0, base_h - 0.1])
            cube([2.0, 4.0, 0.9]);

        // Ventilation & Pin Access Cutouts
        if (lid_style == "gpio_slots") {
            // GPIO Pin Slots (Left and Right)
            for (gx = [-7.62, 7.62]) {
                hull() {
                    translate([gx, -7.5, total_h - roof_t - 1])
                        cylinder(r=1.2, h=roof_t + 3);
                    translate([gx,  7.5, total_h - roof_t - 1])
                        cylinder(r=1.2, h=roof_t + 3);
                }
            }
            // Reset and Boot Pinholes
            translate([-4.0, -4.5, total_h - roof_t - 1])
                cylinder(d=2.0, h=roof_t + 3);
            translate([ 4.0, -4.5, total_h - roof_t - 1])
                cylinder(d=2.0, h=roof_t + 3);

            // User LED Window
            translate([0.0, 2.0, total_h - roof_t - 1])
                cylinder(d=1.8, h=roof_t + 3);
        } else if (lid_style == "minimal") {
            // Minimalist Pinholes only (Reset, Boot, LED)
            translate([-4.0, -4.5, total_h - roof_t - 1])
                cylinder(d=2.0, h=roof_t + 3);
            translate([ 4.0, -4.5, total_h - roof_t - 1])
                cylinder(d=2.0, h=roof_t + 3);
            translate([0.0, 2.0, total_h - roof_t - 1])
                cylinder(d=1.8, h=roof_t + 3);
        }

        // Custom Debossed Text on Top Face
        if (enable_text && text_style == "debossed" && len(custom_text) > 0) {
            translate([text_offset_x, text_offset_y, total_h - text_depth])
                linear_extrude(height=text_depth + 0.1)
                    rotate([0, 0, text_rotation])
                        text(text=custom_text, size=text_size, font=text_font, halign="center", valign="center");
        }
    }

    // Custom Embossed Text on Top Face (Raised)
    if (enable_text && text_style == "embossed" && len(custom_text) > 0) {
        translate([text_offset_x, text_offset_y, total_h])
            linear_extrude(height=text_depth)
                rotate([0, 0, text_rotation])
                    text(text=custom_text, size=text_size, font=text_font, halign="center", valign="center");
    }
}

// ------------------------------------------------------------------------------
// PRINT ORIENTATION & ASSEMBLY LOGIC
// ------------------------------------------------------------------------------
module xiao_case_lid_flat() {
    // Flipped 180 degrees so top roof sits flat on Z = 0 for 100% support-free printing
    translate([0, 0, total_h])
        rotate([180, 0, 0])
            xiao_case_lid_raw();
}

// Color palettes for MakerWorld in-browser preview
c_base = [0.22, 0.24, 0.27];   // Tactical Matte Charcoal
c_lid  = [0.95, 0.45, 0.10];   // Rescue Signal Orange

if (part_mode == "assembled") {
    color(c_base) xiao_case_base();
    color(c_lid)  xiao_case_lid_raw();
} else if (part_mode == "base_only") {
    color(c_base) xiao_case_base();
} else if (part_mode == "lid_only") {
    color(c_lid)  xiao_case_lid_flat();
} else {
    // "both_flat": Both halves lying flat side-by-side on print bed
    offset_x = outer_w/2 + part_spacing/2;
    translate([-offset_x, 0, 0])
        color(c_base) xiao_case_base();
    translate([offset_x, 0, 0])
        color(c_lid)  xiao_case_lid_flat();
}
