#include <stdint.h>
#include <unistd.h>
#include "si3547.h"

#define msleep(x)                usleep((x) * 1000)

//page 0 configuration
const uint8_t p0_addr_06_08_conf[] = {0x00, 0x00, 0x00};
const uint8_t p0_addr_0b_conf      =  0x6c;
const uint8_t p0_addr_16_1a_conf[] = {0x0f, 0xdc, 0x22, 0x00, 0x0f};  /*16 - 1a*/
const uint8_t p0_addr_20_conf      =  0x01;
const uint8_t p0_addr_2b_3d_conf[] = {0x02, 0x0d, 0x51, 0x3b, 0x00,  /*2b - 2f*/
                                      0x00, 0x00, 0x3b, 0x00, 0x3b,  /*30 - 34*/
                                      0x00, 0x3b, 0x00, 0x00, 0x00,  /*35 - 39*/
                                      0x3b, 0x00, 0x3b, 0x00};       /*3a - 3d*/
const uint8_t p0_addr_3f_conf      =  0xdd;
const uint8_t p0_addr_40_69_conf[] = {0x04, 0x0B, 0x00, 0x0d, 0x0d,  /*40 - 44*/
                                      0x0c, 0x32, 0x00, 0x32, 0x32,  /*45 - 49*/
                                      0x32, 0x00, 0x32, 0x32, 0x05,  /*4A - 4E*/
                                      0x55, 0x0f, 0x03, 0x00, 0x03,  /*4F - 53*/
                                      0x03, 0x03, 0x00, 0x03, 0x03,  /*54 - 58*/
                                      0x51, 0xaa, 0x6a, 0x04, 0x01,  /*59 - 5d*/
                                      0x00, 0x00, 0x00, 0x00, 0xaa,  /*5e - 62*/
                                      0x6a, 0x04, 0x01, 0xaa, 0x6a,  /*63 - 67*/
                                      0x04, 0x01};
const uint8_t p0_addr_92_a2_conf[] = {0x0f, 0xaa, 0xaa, 0x00, 0x88,  /*92 - 96*/
                                      0x88, 0x66, 0x66, 0x0f, 0x66,  /*97 - 9b*/
                                      0x66, 0x96, 0x44, 0x44, 0x22,  /*9c - a0*/
                                      0x22, 0x00};
const uint8_t p0_addr_a4_a7_conf[] = {0xb4, 0x61, 0x00, 0x00};
const uint8_t p0_addr_a9_ac_conf[] = {0xde, 0x31, 0x00, 0x00};
const uint8_t p0_addr_ae_b1_conf[] = {0xde, 0x31, 0x00, 0x00};
const uint8_t p0_addr_b3_b6_conf[] = {0xb4, 0x61, 0x00, 0x00};
const uint8_t p0_addr_e5_f5_conf[] = {0xf1, 0x0a, 0x60, 0x00, 0x00,  /*e5 - e9*/
                                      0x05, 0x30, 0x00, 0x00, 0x05,  /*ea - ee*/
                                      0x30, 0x00, 0x00, 0x0a, 0x60,  /*ef - f3*/
                                      0x00, 0x00};

//page 1 configuration
const uint8_t p1_addr_102_conf       =  0x01;
const uint8_t p1_addr_108_10c_conf[] = {0x02, 0xcc, 0x00, 0x08, 0x01};
const uint8_t p1_addr_112_120_conf[] = {0x02, 0xcc, 0x00, 0x08, 0x01,  /*112 - 116*/
                                        0x02, 0x09, 0x6b, 0x08, 0x01,  /*117 - 11b*/
                                        0x02, 0x02, 0x35, 0x08, 0x01}; /*11c - 120*/
const uint8_t p1_addr_126_134_conf[] = {0x02, 0x09, 0x6b, 0x0b, 0x04,  /*126 - 12a*/
                                        0x01, 0x09, 0x3b, 0x28, 0x00,  /*12b - 12f*/
                                        0x02, 0x09, 0x6b, 0x09, 0x02}; /*130 - 134*/
const uint8_t p1_addr_13a_142_conf[] = {0x02, 0x09, 0x6b, 0x0a, 0x03,  /*13a - 13e*/
                                        0x00, 0x00, 0x40, 0xff};

//page 2 configuration
const uint8_t p2_addr_206_conf       =  0x00;
const uint8_t p2_addr_208_22f_conf[] = {0x0d, 0x00, 0x00, 0x00, 0x00,  /*208 - 20c*/
                                        0x00, 0x01, 0x00, 0x00, 0x00,  /*20d - 211*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*212 - 216*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*217 - 21b*/
                                        0x34, 0x00, 0x00, 0x00, 0x00,  /*21c - 220*/
                                        0x00, 0x01, 0x00, 0x00, 0x00,  /*221 - 225*/
                                        0x34, 0x00, 0x00, 0x00, 0x00,  /*226 - 22a*/
                                        0x00, 0x01, 0x00, 0x00, 0x00}; /*226 - 22f*/
const uint8_t p2_addr_231_23e_conf[] = {0x0b, 0x0b, 0x0b, 0x0b, 0x00,  /*231 - 235*/
                                        0x00, 0x00, 0x40, 0x90, 0x00,  /*236 - 23a*/
                                        0x00, 0x00, 0x00, 0x80};
const uint8_t p2_addr_24a_24c_conf[] = {0x0f, 0x00, 0x00};
const uint8_t p2_addr_250_258_conf[] = {0x07, 0x00, 0x00, 0x03, 0x00,  /*250 - 254*/
                                        0x00, 0x03, 0x00, 0x00};
const uint8_t p2_addr_25c_264_conf[] = {0x03, 0x00, 0x00, 0x00, 0x00,  /*25c - 260*/
                                        0x00, 0x03, 0x00, 0x00};
const uint8_t p2_addr_268_272_conf[] = {0x03, 0x00, 0x00, 0x44, 0x32,  /*268 - 26c*/
                                        0x33, 0x41, 0x00, 0x00, 0x00,  /*26d - 271*/
                                        0x00};
const uint8_t p2_addr_28a_291_conf[] = {0x00, 0x00, 0x00, 0x00, 0x00,  /*28a - 28e*/
                                        0x00, 0x00, 0x00};
const uint8_t p2_addr_294_297_conf[] = {0xcb, 0xbc, 0x0f, 0x0f};
const uint8_t p2_addr_299_2b1_conf[] = {0x0f, 0xfa, 0x01, 0x00, 0x19,  /*299 - 29d*/
                                        0x01, 0x00, 0x19, 0x01, 0x00,  /*29e - 2a2*/
                                        0xfa, 0x01, 0x00, 0xcc, 0x04,  /*2a3 - 2a7*/
                                        0x00, 0x66, 0x02, 0x00, 0x66,  /*2a8 - 2ac*/
                                        0x02, 0x00, 0xcc, 0x04, 0x00}; /*2ad - 2b1*/
const uint8_t p2_addr_2b7_conf       =  0xff;

//page 3 configuration
const uint8_t p3_addr_302_32d_conf[] = {0x00, 0x00, 0x00, 0x86, 0x0d,  /*302 - 306*/
                                        0x00, 0x00, 0x00, 0x50, 0xc3,  /*307 - 30b*/
                                        0x00, 0x00, 0x00, 0x00, 0xc3,  /*30c - 310*/
                                        0x06, 0x00, 0x00, 0x80, 0x96,  /*311 - 315*/
                                        0x98, 0x00, 0x00, 0x00, 0x00,  /*316 - 31a*/
                                        0xc3, 0x06, 0x00, 0x00, 0x80,  /*31b - 31f*/
                                        0x96, 0x98, 0x00, 0x00, 0x00,  /*320 - 324*/
                                        0x00, 0x86, 0x0d, 0x00, 0x00,  /*325 - 329*/
                                        0x00, 0x50, 0xc3, 0x00};       /*32a - 32d*/
const uint8_t p3_addr_338_conf       =  0x00;
const uint8_t p3_addr_33b_352_conf[] = {0x00, 0x00, 0x00, 0x00, 0x00,  /*33b - 33f*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*340 - 344*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*345 - 349*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*34a - 34e*/
                                        0x00, 0x00, 0x00, 0x00};       /*34f - 352*/

//page 4 configuration
const uint8_t p4_addr_408_413_conf[] = {0x0f, 0x1e, 0x0c, 0x0b, 0x07,  /*408 - 40c*/
                                        0x3f, 0x12, 0x26, 0x09, 0x08,  /*40d - 411*/
                                        0x07, 0x3f};
const uint8_t p4_addr_415_41f_conf[] = {0x00, 0x00, 0x00, 0x00, 0xd0,  /*415 - 419*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*41a - 41e*/
                                        0x80};
const uint8_t p4_addr_421_42f_conf[] = {0x2b, 0x01, 0x00, 0x00, 0x00,  /*421 - 425*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*426 - 42a*/
                                        0x01, 0x87, 0x03, 0x19, 0x19}; /*42b - 42f*/
const uint8_t p4_addr_431_43e_conf[] = {0x00, 0x63, 0x03, 0x00, 0x00,  /*431 - 435*/
                                        0x04, 0x00, 0x00, 0x00, 0x02,  /*436 - 43a*/
                                        0x03, 0x00, 0x11, 0x06};
const uint8_t p4_addr_442_445_conf[] = {0x00, 0x00, 0x00, 0x00};
const uint8_t p4_addr_489_48a_conf[] = {0x00, 0x00};
const uint8_t p4_addr_49b_conf       =  0xfa;
const uint8_t p4_addr_49d_4a2_conf[] = {0x0f, 0x20, 0x0c, 0x0b, 0x07,  /*49d - 4a1*/
                                        0x3f};
const uint8_t p4_addr_4a6_conf       =  0x03;

//page 5 configuration
const uint8_t p5_addr_508_513_conf[] = {0x0e, 0x1c, 0x0d, 0x0c, 0x07,  /*508 - 50c*/
                                        0x3f, 0x12, 0x27, 0x09, 0x08,  /*50d - 511*/
                                        0x07, 0x3f};
const uint8_t p5_addr_515_51f_conf[] = {0x00, 0x00, 0x00, 0x00, 0x45,  /*515 - 519*/
                                        0x01, 0x00, 0x00, 0x00, 0x00,  /*51a - 51e*/
                                        0x80};
const uint8_t p5_addr_521_52f_conf[] = {0x2b, 0x01, 0x00, 0x00, 0x00,  /*521 - 525*/
                                        0x00, 0x00, 0x00, 0x00, 0x01,  /*526 - 52a*/
                                        0x01, 0x87, 0x03, 0x19, 0x19}; /*52b - 52f*/
const uint8_t p5_addr_531_53e_conf[] = {0x00, 0x63, 0x03, 0x00, 0x00,  /*531 - 535*/
                                        0x04, 0x00, 0x00, 0x00, 0x02,  /*536 - 53a*/
                                        0x03, 0x00, 0x11, 0x06};
const uint8_t p5_addr_542_545_conf[] = {0x00, 0x00, 0x00, 0x00};
const uint8_t p5_addr_589_59b_conf[] = {0x0d, 0x00, 0xfa};
const uint8_t p5_addr_59d_5a2_conf[] = {0x0e, 0x1f, 0x0d, 0x0c, 0x07,  /*59d - 5a1*/
                                        0x3f};
const uint8_t p5_addr_5a6_conf       =  0x03;

//page 6 configuration
const uint8_t p6_addr_602_conf       =  0x01;
const uint8_t p6_addr_608_613_conf[] = {0x0e, 0x1c, 0x0d, 0x0c, 0x07,  /*608 - 60c*/
                                        0x3f, 0x12, 0x27, 0x09, 0x08,  /*60d - 611*/
                                        0x07, 0x3f};
const uint8_t p6_addr_615_61f_conf[] = {0x00, 0x00, 0x00, 0x00, 0x45,  /*615 - 619*/
                                        0x01, 0x00, 0x00, 0x00, 0x00,  /*61a - 61e*/
                                        0x80};
const uint8_t p6_addr_621_62f_conf[] = {0x2b, 0x01, 0x00, 0x00, 0x00,  /*621 - 625*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*626 - 62a*/
                                        0x01, 0x87, 0x03, 0x19, 0x19}; /*62b - 62f*/
const uint8_t p6_addr_631_63e_conf[] = {0x00, 0x63, 0x03, 0x00, 0x00,  /*631 - 635*/
                                        0x04, 0x00, 0x00, 0x00, 0x02,  /*636 - 63a*/
                                        0x03, 0x00, 0x11, 0x06};
const uint8_t p6_addr_642_645_conf[] = {0x00, 0x00, 0x00, 0x00};
const uint8_t p6_addr_689_69b_conf[] = {0x0d, 0x00, 0xfa};
const uint8_t p6_addr_69d_6a2_conf[] = {0x0e, 0x1f, 0x0d, 0x0c, 0x07,  /*69d - 6a1*/
                                        0x3f};
const uint8_t p6_addr_6a6_conf       =  0x03;

//page 7 configuration
const uint8_t p7_addr_702_conf       =  0x01;
const uint8_t p7_addr_709_714_conf[] = {0x0f, 0x1e, 0x0c, 0x0b, 0x07,  /*709 - 70d*/
                                        0x3f, 0x12, 0x26, 0x09, 0x08,  /*70e - 712*/
                                        0x07, 0x3f};
const uint8_t p7_addr_716_720_conf[] = {0x00, 0x00, 0x00, 0x00, 0xd0,  /*716 - 71a*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*71b - 71f*/
                                        0x80};
const uint8_t p7_addr_722_730_conf[] = {0x2b, 0x01, 0x00, 0x00, 0x00,  /*722 - 726*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*727 - 72b*/
                                        0x01, 0x87, 0x03, 0x19, 0x19}; /*72c - 730*/
const uint8_t p7_addr_732_73f_conf[] = {0x00, 0x63, 0x03, 0x00, 0x00,  /*732 - 736*/
                                        0x06, 0x00, 0x03, 0x21, 0x02,  /*737 - 73b*/
                                        0x03, 0x00, 0x11, 0x06};
const uint8_t p7_addr_743_746_conf[] = {0x00, 0x00, 0x00, 0x00};
const uint8_t p7_addr_789_78b_conf[] = {0x0d, 0x00, 0xfa};
const uint8_t p7_addr_79d_7a2_conf[] = {0x0f, 0x20, 0x0c, 0x0b, 0x07,  /*79d - 7a1*/
                                        0x3f};
const uint8_t p7_addr_7a6_conf       =  0x03;

//page 8 configuration
const uint8_t p8_addr_802_861_conf[] = {0x35, 0x04, 0x00, 0x45, 0x04,  /*802 - 806*/
                                        0x00, 0x43, 0x04, 0x00, 0x43,  /*807 - 80b*/
                                        0x04, 0x00, 0x44, 0x04, 0x00,  /*80c - 810*/
                                        0x35, 0x05, 0x00, 0x45, 0x05,  /*811 - 815*/
                                        0x00, 0x42, 0x05, 0x00, 0x43,  /*816 - 81a*/
                                        0x05, 0x00, 0x44, 0x05, 0x00,  /*81b - 81f*/
                                        0x35, 0x06, 0x00, 0x45, 0x06,  /*820 - 824*/
                                        0x00, 0x42, 0x06, 0x00, 0x43,  /*825 - 829*/
                                        0x06, 0x00, 0x44, 0x06, 0x00,  /*82a - 82e*/
                                        0x36, 0x07, 0x00, 0x46, 0x07,  /*82f - 833*/
                                        0x00, 0x43, 0x07, 0x00, 0x44,  /*834 - 838*/
                                        0x07, 0x00, 0x45, 0x07, 0x00,  /*839 - 83d*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*83e - 842*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*843 - 847*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*848 - 84c*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*84d - 851*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*852 - 856*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*857 - 85b*/
                                        0x00, 0x00, 0x00, 0x00, 0x00,  /*85c - 860*/
                                        0x00};

//page 9 configuration
const uint8_t p9_addr_90e_conf       =  0x03;
const uint8_t p9_addr_943_conf       =  0x01;
const uint8_t p9_addr_949_94a_conf[] = {0x0d, 0x0d};
const uint8_t p9_addr_94e_94f_conf[] = {0x49, 0x02};
const uint8_t p9_addr_95e_conf       =  0x00;

//page A configuration
const uint8_t pa_addr_a03_a05_conf[] = {0x0f, 0x00, 0x0f};
//page B configuration
const uint8_t pb_addr_b44_b48_conf[] = {0xff, 0x00, 0x00, 0x02, 0x02};
const uint8_t pb_addr_b4a_conf       =  0x00;
const uint8_t pb_addr_b57_b58_conf[] = {0x07, 0x01};

void start_config_preamble(void)
{
    //argu: page, addr, val
    (void)si3547_page_writeb(0x0b, 0x24, 0xc0);
    (void)si3547_page_writeb(0x0b, 0x25, 0x00);
    //Rev D stuck divider fix
    (void)si3547_page_writeb(0x0b, 0x4e, 0x1a);
}

void end_config_preamble(void)
{
    //argu: page, addr, val
    (void)si3547_page_writeb(0x04, 0x14, 0x01);
    (void)si3547_page_writeb(0x05, 0x14, 0x01);
    (void)si3547_page_writeb(0x06, 0x14, 0x01);
    (void)si3547_page_writeb(0x07, 0x15, 0x01);
    (void)si3547_page_writeb(0x00, 0x1c, 0x01);
    (void)si3547_page_writeb(0x0b, 0x24, 0xc3);
    (void)si3547_page_writeb(0x0b, 0x25, 0x02);
}

#define PAGE0_INIT() \
do{ \
    (void)si3547_update_page(PAGE0, 0x06, p0_addr_06_08_conf, sizeof(p0_addr_06_08_conf)); \
    (void)si3547_page_writeb(PAGE0, 0x0b, p0_addr_0b_conf);                                \
    (void)si3547_update_page(PAGE0, 0x16, p0_addr_16_1a_conf, sizeof(p0_addr_16_1a_conf)); \
    (void)si3547_page_writeb(PAGE0, 0x20, p0_addr_20_conf);                                \
    (void)si3547_update_page(PAGE0, 0x2b, p0_addr_2b_3d_conf, sizeof(p0_addr_2b_3d_conf)); \
    (void)si3547_page_writeb(PAGE0, 0x3f, p0_addr_3f_conf);                                \
    (void)si3547_update_page(PAGE0, 0x40, p0_addr_40_69_conf, sizeof(p0_addr_40_69_conf)); \
    (void)si3547_update_page(PAGE0, 0x92, p0_addr_92_a2_conf, sizeof(p0_addr_92_a2_conf)); \
    (void)si3547_update_page(PAGE0, 0xa4, p0_addr_a4_a7_conf, sizeof(p0_addr_a4_a7_conf)); \
    (void)si3547_update_page(PAGE0, 0xa9, p0_addr_a9_ac_conf, sizeof(p0_addr_a9_ac_conf)); \
    (void)si3547_update_page(PAGE0, 0xae, p0_addr_ae_b1_conf, sizeof(p0_addr_ae_b1_conf)); \
    (void)si3547_update_page(PAGE0, 0xb3, p0_addr_b3_b6_conf, sizeof(p0_addr_b3_b6_conf)); \
    (void)si3547_update_page(PAGE0, 0xe5, p0_addr_e5_f5_conf, sizeof(p0_addr_e5_f5_conf)); \
\
}while(0)

#define PAGE1_INIT() \
do{ \
    (void)si3547_page_writeb(PAGE1, 0x02, p1_addr_102_conf);                                   \
    (void)si3547_update_page(PAGE1, 0x08, p1_addr_108_10c_conf, sizeof(p1_addr_108_10c_conf)); \
    (void)si3547_update_page(PAGE1, 0x12, p1_addr_112_120_conf, sizeof(p1_addr_112_120_conf)); \
    (void)si3547_update_page(PAGE1, 0x26, p1_addr_126_134_conf, sizeof(p1_addr_126_134_conf)); \
    (void)si3547_update_page(PAGE1, 0x3a, p1_addr_13a_142_conf, sizeof(p1_addr_13a_142_conf)); \
\
}while(0)

#define PAGE2_INIT() \
do{ \
    (void)si3547_page_writeb(PAGE2, 0x06, p2_addr_206_conf);                                   \
    (void)si3547_update_page(PAGE2, 0x08, p2_addr_208_22f_conf, sizeof(p2_addr_208_22f_conf)); \
    (void)si3547_update_page(PAGE2, 0x31, p2_addr_231_23e_conf, sizeof(p2_addr_231_23e_conf)); \
    (void)si3547_update_page(PAGE2, 0x4a, p2_addr_24a_24c_conf, sizeof(p2_addr_24a_24c_conf)); \
    (void)si3547_update_page(PAGE2, 0x50, p2_addr_250_258_conf, sizeof(p2_addr_250_258_conf)); \
    (void)si3547_update_page(PAGE2, 0x5c, p2_addr_25c_264_conf, sizeof(p2_addr_25c_264_conf)); \
    (void)si3547_update_page(PAGE2, 0x68, p2_addr_268_272_conf, sizeof(p2_addr_268_272_conf)); \
    (void)si3547_update_page(PAGE2, 0x8a, p2_addr_28a_291_conf, sizeof(p2_addr_28a_291_conf)); \
    (void)si3547_update_page(PAGE2, 0x94, p2_addr_294_297_conf, sizeof(p2_addr_294_297_conf)); \
    (void)si3547_update_page(PAGE2, 0x99, p2_addr_299_2b1_conf, sizeof(p2_addr_299_2b1_conf)); \
    (void)si3547_page_writeb(PAGE2, 0xb7, p2_addr_2b7_conf);                                   \
\
}while(0)

#define PAGE3_INIT() \
do{ \
    (void)si3547_update_page(PAGE3, 0x02, p3_addr_302_32d_conf, sizeof(p3_addr_302_32d_conf)); \
    (void)si3547_page_writeb(PAGE2, 0x38, p3_addr_338_conf);                                   \
    (void)si3547_update_page(PAGE3, 0x3b, p3_addr_33b_352_conf, sizeof(p3_addr_33b_352_conf)); \
}while(0)

#define PAGE4_INIT() \
do{ \
    (void)si3547_update_page(PAGE4, 0x08, p4_addr_408_413_conf, sizeof(p4_addr_408_413_conf)); \
    (void)si3547_update_page(PAGE4, 0x15, p4_addr_415_41f_conf, sizeof(p4_addr_415_41f_conf)); \
    (void)si3547_update_page(PAGE4, 0x21, p4_addr_421_42f_conf, sizeof(p4_addr_421_42f_conf)); \
    (void)si3547_update_page(PAGE4, 0x31, p4_addr_431_43e_conf, sizeof(p4_addr_431_43e_conf)); \
    (void)si3547_update_page(PAGE4, 0x42, p4_addr_442_445_conf, sizeof(p4_addr_442_445_conf)); \
    (void)si3547_update_page(PAGE4, 0x89, p4_addr_489_48a_conf, sizeof(p4_addr_489_48a_conf)); \
    (void)si3547_page_writeb(PAGE4, 0x9b, p4_addr_49b_conf);                                   \
    (void)si3547_update_page(PAGE4, 0x9d, p4_addr_49d_4a2_conf, sizeof(p4_addr_49d_4a2_conf)); \
    (void)si3547_page_writeb(PAGE4, 0xa6, p4_addr_4a6_conf);                                   \
\
}while(0)

#define PAGE5_INIT() \
do{ \
    (void)si3547_update_page(PAGE5, 0x08, p5_addr_508_513_conf, sizeof(p5_addr_508_513_conf)); \
    (void)si3547_update_page(PAGE5, 0x15, p5_addr_515_51f_conf, sizeof(p5_addr_515_51f_conf)); \
    (void)si3547_update_page(PAGE5, 0x21, p5_addr_521_52f_conf, sizeof(p5_addr_521_52f_conf)); \
    (void)si3547_update_page(PAGE5, 0x31, p5_addr_531_53e_conf, sizeof(p5_addr_531_53e_conf)); \
    (void)si3547_update_page(PAGE5, 0x42, p5_addr_542_545_conf, sizeof(p5_addr_542_545_conf)); \
    (void)si3547_update_page(PAGE5, 0x89, p5_addr_589_59b_conf, sizeof(p5_addr_589_59b_conf)); \
    (void)si3547_update_page(PAGE5, 0x9d, p5_addr_59d_5a2_conf, sizeof(p5_addr_59d_5a2_conf)); \
    (void)si3547_page_writeb(PAGE5, 0xa6, p5_addr_5a6_conf);                                   \
\
}while(0)

#define PAGE6_INIT() \
do{ \
    (void)si3547_page_writeb(PAGE6, 0x02, p6_addr_602_conf);                                   \
    (void)si3547_update_page(PAGE6, 0x08, p6_addr_608_613_conf, sizeof(p6_addr_608_613_conf)); \
    (void)si3547_update_page(PAGE6, 0x15, p6_addr_615_61f_conf, sizeof(p6_addr_615_61f_conf)); \
    (void)si3547_update_page(PAGE6, 0x21, p6_addr_621_62f_conf, sizeof(p6_addr_621_62f_conf)); \
    (void)si3547_update_page(PAGE6, 0x31, p6_addr_631_63e_conf, sizeof(p6_addr_631_63e_conf)); \
    (void)si3547_update_page(PAGE6, 0x42, p6_addr_642_645_conf, sizeof(p6_addr_642_645_conf)); \
    (void)si3547_update_page(PAGE6, 0x89, p6_addr_689_69b_conf, sizeof(p6_addr_689_69b_conf)); \
    (void)si3547_update_page(PAGE6, 0x9d, p6_addr_69d_6a2_conf, sizeof(p6_addr_69d_6a2_conf)); \
    (void)si3547_page_writeb(PAGE6, 0xa6, p6_addr_6a6_conf);                                   \
\
}while(0)

#define PAGE7_INIT() \
do{ \
    (void)si3547_page_writeb(PAGE7, 0x02, p7_addr_702_conf);                                   \
    (void)si3547_update_page(PAGE7, 0x09, p7_addr_709_714_conf, sizeof(p7_addr_709_714_conf)); \
    (void)si3547_update_page(PAGE7, 0x16, p7_addr_716_720_conf, sizeof(p7_addr_716_720_conf)); \
    (void)si3547_update_page(PAGE7, 0x22, p7_addr_722_730_conf, sizeof(p7_addr_722_730_conf)); \
    (void)si3547_update_page(PAGE7, 0x32, p7_addr_732_73f_conf, sizeof(p7_addr_732_73f_conf)); \
    (void)si3547_update_page(PAGE7, 0x43, p7_addr_743_746_conf, sizeof(p7_addr_743_746_conf)); \
    (void)si3547_update_page(PAGE7, 0x89, p7_addr_789_78b_conf, sizeof(p7_addr_789_78b_conf)); \
    (void)si3547_update_page(PAGE7, 0x9d, p7_addr_79d_7a2_conf, sizeof(p7_addr_79d_7a2_conf)); \
    (void)si3547_page_writeb(PAGE7, 0xa6, p7_addr_7a6_conf);                                   \
\
}while(0)

#define PAGE8_INIT() \
do{ \
    (void)si3547_update_page(PAGE8, 0x02, p8_addr_802_861_conf, sizeof(p8_addr_802_861_conf)); \
}while(0)

#define PAGE9_INIT() \
do{ \
    (void)si3547_page_writeb(PAGE9, 0x0e, p9_addr_90e_conf);                                   \
    (void)si3547_page_writeb(PAGE9, 0x43, p9_addr_943_conf);                                   \
    (void)si3547_update_page(PAGE9, 0x49, p9_addr_949_94a_conf, sizeof(p9_addr_949_94a_conf)); \
    (void)si3547_update_page(PAGE9, 0x4e, p9_addr_94e_94f_conf, sizeof(p9_addr_94e_94f_conf)); \
    (void)si3547_page_writeb(PAGE9, 0x5e, p9_addr_95e_conf);                                   \
\
}while(0)

#define PAGEA_INIT() \
do{ \
    (void)si3547_update_page(PAGEA, 0x03, pa_addr_a03_a05_conf, sizeof(pa_addr_a03_a05_conf)); \
\
}while(0)

#define PAGEB_INIT() \
do{ \
    (void)si3547_update_page(PAGEB, 0x44, pb_addr_b44_b48_conf, sizeof(pb_addr_b44_b48_conf)); \
    (void)si3547_page_writeb(PAGEB, 0x4a, pb_addr_b4a_conf);                                   \
    (void)si3547_update_page(PAGEB, 0x57, pb_addr_b57_b58_conf, sizeof(pb_addr_b57_b58_conf)); \
\
}while(0)

#define SI3547_INIT_PAGE() \
do { \
    PAGE0_INIT(); \
    PAGE1_INIT(); \
    PAGE2_INIT(); \
    PAGE3_INIT(); \
    PAGE4_INIT(); \
    PAGE5_INIT(); \
    PAGE6_INIT(); \
    PAGE7_INIT(); \
    PAGE8_INIT(); \
    PAGE9_INIT(); \
    PAGEA_INIT(); \
    PAGEB_INIT(); \
}while(0)

void si3547_init(void)
{
    start_config_preamble();
    msleep(300);
    SI3547_INIT_PAGE();
    end_config_preamble();
}
