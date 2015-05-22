// All measurements in mm and stated as L x W x D
// Yun = 73 x 53 x ?
yun_l = 73;
yun_w = 53;
yun_h = 10;

// switch = 8mm diameter
switch_d = 8;
switch_h = 20; //including tabs

// RFID = 62 x 82 x ?
rfid_l = 62;
rfid_w = 82;
rfid_h = 5;

// LCD = 37 x 116 x ?
lcd_l = 37;
lcd_w = 116;
lcd_h = 10;

case_a1 = 233;
case_a2 = 214;
case_b1 = 113;
case_b2 = 76;
case_c1 = 25;
case_c2 = 96;

screw_r = 1.5;

// stuff on the base
translate([75,0,0]) rotate([0,0,90]) yun();

// stuff on the lid
translate([-40,0,97])
{
    translate([110,0,-5]) rfid();
    translate([10,-30,0])
        {
        translate([0,50,0]) rotate([0,0,90]) lcd();
        translate([-40,0,0]) switch();
        translate([40,0,0]) switch();
        }
}

case();

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

module switch()
{
    translate([0,0,-switch_h/2])
    cylinder(r=switch_d/2, h=switch_h, center=true);
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

