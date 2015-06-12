// make holes nice
$fa=10;
$fs=0.8;

// All measurements in mm and stated as L x W x D
// Yun = 73 x 53 x ?
yun_l = 75;
yun_w = 80;
yun_h = 15;
yun_b_l = 67.31;
yun_b_w = 72.39;

// IEC
iec_l = 27.5;
iec_w = 19.5;
iec_h = 25;
iec_bolt_d = 3.5;
iec_bolt_space = 40;

// psu
psu_l = 115;
psu_w = 50;
psu_h = 15;
// bolt spacing
psu_b_l = 108;
psu_b_w = 43;
psu_b_d = 3.5;


// button = 16mm diameter
button_d = 16;
button_h = 20; //including tabs

//encoder
encoder_d = 9;
encoder_h = 20;
encoder_knob_d = 20;
encoder_knob_h = 10;

// RFID = 62 x 82 x ?
rfid_l = 60;
rfid_w = 40;
rfid_h = 3;

// LCD = 37 x 116 x ?
lcd_l = 91.4;
lcd_w = 31.3;
lcd_h = 10;

// dimensions and names from the datasheet
case_a1 = 243.4;
case_a2 = 224.6;
case_b1 = 153.4;
case_b2 = 115.6;
case_c1 = 15;
case_c2 = 51;

screw_r = 1.5;

//uncomment these to make the DXFs
//projection() back_panel();
//projection() front_panel();
//projection()
 front_panel_mount();

//the 3 panels that we need

module back_panel()
{
    difference()
    {
        translate([0,0,-3]) mount_plate();
        // stuff on the base
        translate([0,60,0]) rotate([0,0,90])yun();
        translate([0,-60,0]) psu();
    }
}

module front_panel_mount()
{
    difference()
    {
        mount_plate();
        translate([0,70,-5]) rotate([0,0,-90]) rfid();
        translate([0,20,3]) lcd_mount();
        translate([0,-20,0])
        {
            translate([-30,0,0]) cylinder(r=15,h=10,center=true);
            translate([30,0,0]) cylinder(r=15,h=10,center=true);
        }
    }
}

module front_panel()
{
    // stuff on the lid
    case_holes(); //for alignment
    translate([0,0,case_c2])
    {
        translate([0,20,0]) lcd();
        translate([0,-20,0])
        {
            translate([-30,0,0]) button();
            translate([30,0,0]) encoder();
        }
    }
}

/***********************************************/
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
    yun_b_d = 3.5;

    //fixings
    translate([-yun_b_l/2,-yun_b_w/2,yun_h/2])
        cylinder(r=yun_b_d/2, h=yun_h*2, center=true);
    translate([+yun_b_l/2,-yun_b_w/2,yun_h/2])
        cylinder(r=yun_b_d/2, h=yun_h*2, center=true);
    translate([+yun_b_l/2,+yun_b_w/2,yun_h/2])
        cylinder(r=yun_b_d/2, h=yun_h*2, center=true);
    translate([-yun_b_l/2,+yun_b_w/2,yun_h/2])
        cylinder(r=yun_b_d/2, h=yun_h*2, center=true);
}

module lcd_mount()
{
    translate([0,0,-lcd_h/2])
        cube([lcd_l*1.1, lcd_w*1.1, lcd_h], center=true);  
    //cut outs for the IDC connectors
    cube([116,24,lcd_h],center=true);
    b_l = 108;
    b_w = 29;
    b_h = 10;
    b_d = 3.5;
    translate([-b_l/2,-b_w/2,b_h/2])
        cylinder(r=b_d/2, h=b_h*2, center=true);
    translate([+b_l/2,-b_w/2,b_h/2])
        cylinder(r=b_d/2, h=b_h*2, center=true);
    translate([+b_l/2,+b_w/2,b_h/2])
        cylinder(r=b_d/2, h=b_h*2, center=true);
    translate([-b_l/2,+b_w/2,b_h/2])
        cylinder(r=b_d/2, h=b_h*2, center=true);

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

//the small 13.5MHz one
module rfid()
{
    translate([-6,0,-rfid_h/2])
        cube([rfid_l, rfid_w, rfid_h], center=true);
    translate([-21,0,6])
        cube([30,26,rfid_h*2],center=true);

    b_l = 37;
    b_w1 = 34.5;
    b_w2 = 25.0;
    b_h = 10;
    b_d = 3.5;
    translate([-b_l/2,-b_w1/2,b_h/2])
        cylinder(r=b_d/2, h=b_h*2, center=true);
    translate([-b_l/2,+b_w1/2,b_h/2])
        cylinder(r=b_d/2, h=b_h*2, center=true);
    translate([+b_l/2,-b_w2/2,b_h/2])
        cylinder(r=b_d/2, h=b_h*2, center=true);
    translate([+b_l/2,+b_w2/2,b_h/2])
        cylinder(r=b_d/2, h=b_h*2, center=true);
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
//   translate([0,0,encoder_h/2-encoder_knob_h/2])
//   cylinder(r=encoder_knob_d/2, h=encoder_knob_h, center=true);
}

module case_holes()
{
	w = 238;
	l = 148;
    bd = 1;
    translate([-l/2,-w/2,0])
        cylinder(r=bd/2, h=10, center=true);
    translate([+l/2,+w/2,0])
        cylinder(r=bd/2, h=10, center=true);
}

module mount_plate()
{
    mp_w = 191.0;
    mp_l = 128;
    mp_bd = 3.5;
    cut_r =8;
    w = 210;
    l = 140;
    difference()
    {
        cube([l,w,3],center=true);
        translate([l/2,0,0])cylinder(r=cut_r,h=10,center=true);
        translate([-l/2,0,0])cylinder(r=cut_r,h=10,center=true);
        translate([-mp_l/2,-mp_w/2,0])
            cylinder(r=mp_bd/2, h=10, center=true);
        translate([+mp_l/2,-mp_w/2,0])
            cylinder(r=mp_bd/2, h=10, center=true);
        translate([+mp_l/2,+mp_w/2,0])
            cylinder(r=mp_bd/2, h=10, center=true);
        translate([-mp_l/2,+mp_w/2,0])
            cylinder(r=mp_bd/2, h=10, center=true);
    }
}

module case()
{
	rotate([0,0,90])
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
}

