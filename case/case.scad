// All measurements in mm and stated as L x W x D
// Yun = 73 x 53 x ?
yun_l = 73;
yun_w = 53;
yun_h = 10;

// IEC
iec_l = 27;
iec_w = 20;
iec_h = 20;
iec_bolt_d = 3.5;
iec_bolt_space = 35;

// psu
psu_l = 115;
psu_w = 50;
psu_h = 15;
// bolt spacing
psu_b_l = 108;
psu_b_w = 43;
psu_b_d = 3;


// button = 16mm diameter
button_d = 16;
button_h = 20; //including tabs

//encoder
encoder_d = 8;
encoder_h = 20;
encoder_knob_d = 20;
encoder_knob_h = 10;

// RFID = 62 x 82 x ?
rfid_l = 62;
rfid_w = 82;
rfid_h = 5;

// LCD = 37 x 116 x ?
lcd_l = 116;
lcd_w = 37;
lcd_h = 10;

// dimensions and names from the datasheet
case_a1 = 243.4;
case_a2 = 224.6;
case_b1 = 153.4;
case_b2 = 115.6;
case_c1 = 15;
case_c2 = 51;

screw_r = 1.5;

// stuff on the base
translate([0,50,0]) yun();

translate([0,-70,0]) psu();

// stuff on the lid
translate([0,0,case_c2])
{
    translate([0,80,-5]) rotate([0,0,90]) rfid();
    translate([0,20,0]) lcd();
    translate([0,-20,0])
    {
        translate([-30,0,0]) button();
        translate([30,0,0]) encoder();
    }
}

// side
translate([0,-case_a1/2,15])
    rotate([90,0,0]) iec();

rotate([0,0,90])
    case();

// modules
module iec()
{

    cube([iec_l, iec_w, iec_h], center=true);
    translate([-iec_bolt_space/2,0,0])
        cylinder(r=iec_bolt_d/2, h=iec_h, center=true);
    translate([iec_bolt_space/2,0,0])
        cylinder(r=iec_bolt_d/2, h=iec_h, center=true);

}

module yun()
{
    translate([0,0,yun_h/2])
    cube([yun_l, yun_w, yun_h], center=true);
}


module lcd()
{
    translate([0,0,-lcd_h/2])
    cube([lcd_l, lcd_w, lcd_h], center=true);
}

module psu()
{
    translate([0,0,psu_h/2])
    cube([psu_l, psu_w, psu_h], center=true);
    translate([-psu_b_l/2,-psu_b_w/2,psu_h/2])
        cylinder(r=psu_b_d/2, h=psu_h*2, center=true);
    translate([+psu_b_l/2,-psu_b_w/2,psu_h/2])
        cylinder(r=psu_b_d/2, h=psu_h*2, center=true);
    translate([+psu_b_l/2,+psu_b_w/2,psu_h/2])
        cylinder(r=psu_b_d/2, h=psu_h*2, center=true);
    translate([-psu_b_l/2,+psu_b_w/2,psu_h/2])
        cylinder(r=psu_b_d/2, h=psu_h*2, center=true);
}
module rfid()
{
    translate([0,0,-rfid_h/2])
    cube([rfid_l, rfid_w, rfid_h], center=true);
}

module led()
{
    translate([0,0,-led_h/2])
    cylinder(r=led_d/2, h=led_h, center=true);
}

module button()
{
    translate([0,0,-button_h/2])
    cylinder(r=button_d/2, h=button_h, center=true);
}
module encoder()
{
    translate([0,0,-encoder_h/2])
    cylinder(r=encoder_d/2, h=encoder_h, center=true);
    translate([0,0,encoder_h/2-encoder_knob_h/2])
    cylinder(r=encoder_knob_d/2, h=encoder_knob_h, center=true);
}

module case()
{
    color("red", 0.2)
        translate([0,0,(case_c2-case_c1)/2])
        {
            cube([case_a1, case_b2, case_c2-case_c1], center=true);
            cube([case_a2, case_b1, case_c2-case_c1], center=true);
        }
    color("blue", 0.2)
        translate([0,0,case_c2/2-15 + case_c1 *2 ])
        {
            cube([case_a1, case_b2, case_c1], center=true);
            cube([case_a2, case_b1, case_c1], center=true);
        }
}

