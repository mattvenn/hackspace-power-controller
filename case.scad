// All measurements in mm and stated as L x W x D
// Yun = 73 x 53 x ?
yun_l = 73;
yun_w = 53;
yun_h = 10;

// SSR + heatsink = 100 x 55 x 67
ssr_l = 100;
ssr_w = 55;
ssr_h = 67;

// PSU = 105 x 50 x 18
psu_l = 105;
psu_w = 50;
psu_h = 18;

// LED = 12mm diameter
led_d = 12;
led_h = 15;

// RFID = 62 x 82 x ?
rfid_l = 62;
rfid_w = 82;
rfid_h = 5;

// IEC inlet/outlet/fuse = 32.5 x 79.2 (hole size for 1.5mm panel thickness) x 32.3 depth
iec_l = 32.5;
iec_w = 79.2;
iec_h = 32.3;

// LCD = 37 x 116 x ?
lcd_l = 37;
lcd_w = 116;
lcd_h = 10;

case_l = 240;
case_w = 120;
case_h = 100;

screw_r = 1.5;

// stuff on the base
translate([0,0,0]) rotate([0,0,90]) ssr();
translate([-60,0,0]) rotate([0,0,90]) psu();
translate([-110,0,30])
    rotate([0,0,0]) iec();
translate([70,0,0]) rotate([0,0,90]) yun();

// stuff on the lid
translate([-40,0,90])
{
    translate([110,0,0]) rfid();
    translate([0,20,0])
        {
        translate([-25,-40,0]) led();
        translate([25,-40,0]) led();
        translate([0,0,0]) rotate([0,0,90]) lcd();
        }
}

case();

/*
projection()
difference()
{
    base();
    translate([-base_w/2 + 25,0,0])
        ssr();
    translate([-base_w/2 + 25 + 50,0,0])
        ac_dc();
    translate([-base_w/2 + 25 + 50 + 60,0,0])
        arduino();
    translate([-base_w/2 + 25 + 50 + 60 + 50,0,0])
        temp();
}
//arduino();
//temp();
*/

module psu()
{
    translate([0,0,psu_h/2])
        cube([psu_l, psu_w, psu_h], center=true);
    /*
    translate([ac_dc_w/2,ac_dc_h/2,0])
        cylinder(r=screw_r,h=th*2,center=true);
    translate([-ac_dc_w/2,ac_dc_h/2,0])
        cylinder(r=screw_r,h=th*2,center=true);
    translate([ac_dc_w/2,-ac_dc_h/2,0])
        cylinder(r=screw_r,h=th*2,center=true);
    translate([-ac_dc_w/2,-ac_dc_h/2,0])
        cylinder(r=screw_r,h=th*2,center=true);
    */
}
module yun()
{
    translate([0,0,yun_h/2])
    cube([yun_l, yun_w, yun_h], center=true);
}

module ssr()
{
    translate([0,0,ssr_h/2])
    cube([ssr_l, ssr_w, ssr_h], center=true);
}

module lcd()
{
    translate([0,0,-lcd_h/2])
    cube([lcd_l, lcd_w, lcd_h], center=true);
}

module rfid()
{
    translate([0,0,-rfid_h/2])
    cube([rfid_l, rfid_w, rfid_h], center=true);
}

module iec()
{
    translate([0,0,iec_h/2])
    cube([iec_l, iec_w, iec_h], center=true);
}

module led()
{
    translate([0,0,-led_h/2])
    cylinder(r=led_d/2, h=led_h, center=true);
}

module case()
{
    color("red", 0.2)
        translate([0,0,case_h/2-10])
        cube([case_l, case_w, case_h], center=true);
}
