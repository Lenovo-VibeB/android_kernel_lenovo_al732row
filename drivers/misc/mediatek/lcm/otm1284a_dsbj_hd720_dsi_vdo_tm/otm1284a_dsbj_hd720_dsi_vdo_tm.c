#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

#if 1//chenjun
#include <linux/kernel.h>
#include <linux/module.h>  
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
//#include <linux/jiffies.h>
#include <linux/uaccess.h>
//#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#endif
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */

#define FRAME_WIDTH  (720)
#define FRAME_HEIGHT (1280)

/* --------------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------------- */

static LCM_UTIL_FUNCS lcm_util = { 0 };

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))
#define REGFLAG_DELAY								0XFEFF
#define REGFLAG_END_OF_TABLE							0xFFFF	/* END OF REGISTERS MARKER */

//#define LCM_DSI_CMD_MODE									0

#define _LCM_DEBUG_

#ifdef BUILD_LK
#define printk printf
#endif

#ifdef _LCM_DEBUG_
#define lcm_debug(fmt, args...) printk(fmt, ##args)
#else
#define lcm_debug(fmt, args...) do { } while (0)
#endif

#ifdef _LCM_INFO_
#define lcm_info(fmt, args...) printk(fmt, ##args)
#else
#define lcm_info(fmt, args...) do { } while (0)
#endif
#define lcm_err(fmt, args...) printk(fmt, ##args)
/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) lcm_util.dsi_write_regs(addr, pdata, byte_nums)
/* #define read_reg lcm_util.dsi_read_reg() */
#define read_reg_v2(cmd, buffer, buffer_size) lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define LCM_ID_OTM1284A_DSBJ 0x1284
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
#if 1//chenjun
#define FPGA_EARLY_PORTING

#ifndef FPGA_EARLY_PORTING
static int tps65132_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int tps65132_i2c_remove(struct i2c_client *client);

#define I2C_ID_NAME "tps65132"
#if !defined(CONFIG_MTK_LEGACY)
static const struct of_device_id lcm_of_match[] = {
			{.compatible = "mediatek,I2C_LCD_BIAS"},
			{},
};
static struct i2c_client *tps65132_i2c_client = NULL;

static const struct i2c_device_id tps65132_id[] = {
			{ I2C_ID_NAME, 0 },
			{ }
};
#endif

static struct i2c_driver tps65132_i2c_driver = {
	.driver = {
/* .owner          = THIS_MODULE, */
		   .name = "tps65132",
		   .of_match_table = lcm_of_match,
		   },
	.probe = tps65132_i2c_probe,
	.remove = tps65132_i2c_remove,
	.id_table = tps65132_id,
/* .address_data = &bma222_addr_data, */
};

static int tps65132_write_bytes(unsigned char addr, unsigned char value)
{
	int ret = 0;
	struct i2c_client *client = tps65132_i2c_client;
	char write_data[2] = { 0 };

	write_data[0] = addr;
	write_data[1] = value;
	ret = i2c_master_send(client, write_data, 2);
	if (ret < 0)
		printk("tps65132 write data fail !!\n");
	return ret;
}

static int tps65132_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk( "tps65132_iic_probe\n");
	printk("TPS: info==>name=%s addr=0x%x\n",client->name,client->addr);
	tps65132_i2c_client = client; 
	printk("chenjun=>client addr=%d",tps65132_i2c_client->addr);
	return 0;  
}

static int tps65132_i2c_remove(struct i2c_client *client)
{
	printk( "tps65132_remove\n");
	tps65132_i2c_client = NULL;
	i2c_unregister_device(client);
	return 0;
}

static int __init tps65132_i2c_init(void)
{
	printk( "tps65132_iic_init\n");
	#if defined(CONFIG_MTK_LEGACY)
	i2c_register_board_info(1, &tps65132_board_info, 1);
	#endif
	printk( "sgm3804_iic_init2\n");
	i2c_add_driver(&tps65132_i2c_driver);
	printk( "tps65132_iic_init success\n");	
	return 0;
}

static void __exit tps65132_i2c_exit(void)
{
	printk( "tps65132_iic_exit\n");
	i2c_del_driver(&tps65132_i2c_driver);  
}

module_init(tps65132_i2c_init);
module_exit(tps65132_i2c_exit);

MODULE_AUTHOR("Xiaokuan Shi");
MODULE_DESCRIPTION("MTK tps65132 I2C Driver");
MODULE_LICENSE("GPL"); 

#else

#ifdef BUILD_LK
extern int TPS65132_write_byte(kal_uint8 addr, kal_uint8 value);
#endif
extern int tps65132_write_bytes(unsigned char addr, unsigned char value);
extern int disp_gpio_set(int PinType,int Val);
#endif


typedef enum {
	DISP_ENN,
	DISP_ENP,
} PinType;

#endif



/* #define LCM_DSI_CMD_MODE */

struct LCM_setting_table {
	unsigned cmd;
	unsigned char count;
	unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {

	{0x00, 1,{0x00}},
	{0xff, 3,{0x12,0x84,0x01}},	//EXTC=1
	{0x00, 1,{0x80}},	        //Orise mode enable
	{0xff, 2,{0x12,0x84}},

	{0x00, 1,{0x92}},
	{0xff, 2,{0x30,0x02}},		//MIPI 4 Lane

//-------------------- panel setting --------------------//
	{0x00, 1,{0x80}},             //TCON Setting
	{0xc0, 9,{0x00,0x64,0x00,0x10,0x10,0x00,0x64,0x10,0x10}},

	{0x00, 1,{0x90}},             //Panel Timing Setting
	{0xc0, 6,{0x00,0x4B,0x00,0x01,0x00,0x04}},

	{0x00, 1,{0xb3}},             //Interval Scan Frame: 0 frame, column inversion
	{0xc0, 1,{0x00,0x55}},

	{0x00, 1,{0x81}},             //frame rate:60Hz
	{0xc1, 1,{0x55}},

//-------------------- power setting --------------------//
	{0x00, 1,{0xa0}},             //dcdc setting
	{0xc4, 14,{0x05,0x10,0x04,0x02,0x05,0x15,0x11,0x05,0x10,0x07,0x02,0x05,0x15,0x11}},

	{0x00, 1,{0xb0}},             //clamp voltage setting
	{0xc4, 2,{0x00,0x00}},

	{0x00, 1,{0x91}},             //VGH=18V, VGL=-10V, pump ratio:VGH=8x, VGL=-5x
	{0xc5, 2,{0xa6,0xd2}},

	{0x00, 1,{0x00}},             //GVDD=5.008V, NGVDD=-5.008V
	{0xd8, 2,{0xB6,0xB6}},     

	{0x00, 1,{0x00}},             //VCOM=-0.9V
	{0xd9, 1,{0x3C}},             

	{0x00, 1,{0xb3}},             //VDD_18V=1.7V, LVDSVDD=1.6V
	{0xc5, 1,{0x84}},

	{0x00, 1,{0xbb}},             //LVD voltage level setting
	{0xc5, 1,{0x8a}},

	{0x00, 1,{0x82}},		//chopper
	{0xC4, 1,{0x0a}},

	{0x00, 1,{0xc6}},		//debounce
	{0xb0, 1,{0x03}},

	{0x00, 1,{0xc2}},             //precharge disable
	{0xf5, 1,{0x40}},

	{0x00, 1,{0xc3}},             //sample hold gvdd
	{0xf5, 1,{0x85}},
	
	{0x00, 1,{0x87}},             //en op
	{0xc4, 1,{0x18}},

//-------------------- control setting --------------------//
	{0x00, 1,{0x00}},             //ID1
	{0xd0, 1,{0x40}},

	{0x00, 1,{0x00}},             //ID2, ID3
	{0xd1, 2,{0x00,0x00}},

//-------------------- power on setting --------------------//
	{0x00, 1,{0xb2}},             //VGLO1
	{0xf5, 2,{0x00,0x00}},

	{0x00, 1,{0xb6}},             //VGLO2
	{0xf5, 2,{0x00,0x00}},

	{0x00, 1,{0x94}},             //VCL pump dis
	{0xf5, 2,{0x00,0x00}},

	{0x00, 1,{0xd2}},             //VCL reg. en
	{0xf5, 2,{0x06,0x15}},

	{0x00, 1,{0xb4}},             //VGLO1/2 Pull low setting
	{0xc5, 1,{0xcc}},	       //d[7] vglo1 d[6] vglo2 => 0: pull vss, 1: pull vgl

//-------------------- for Power IC ---------------------------------
	{0x00, 1,{0x90}},             //Mode-3
	{0xf5, 4,{0x02,0x11,0x02,0x15}},

	{0x00, 1,{0x90}},             //2xVPNL, 1.5*=00, 2*=50, 3*=a0
	{0xc5, 1,{0x50}},

	{0x00, 1,{0x94}},             //Frequency
	{0xc5, 1,{0x66}},

//-------------------- panel timing state control --------------------//
	{0x00, 1,{0x80}},             //panel timing state control
	{0xcb, 11,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1,{0x90}},             //panel timing state control
	{0xcb, 15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1,{0xa0}},             //panel timing state control
	{0xcb, 15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1,{0xb0}},             //panel timing state control
	{0xcb, 15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1,{0xc0}},             //panel timing state control
	{0xcb, 15,{0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x05,0x00,0x00,0x00,0x00}},

	{0x00, 1,{0xd0}},             //panel timing state control
	{0xcb, 15,{0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05}},

	{0x00, 1,{0xe0}},             //panel timing state control
	{0xcb, 15,{0x05,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x00}},

	{0x00, 1,{0xf0}},             //panel timing state control
	{0xcb, 15,{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}},

//-------------------- panel pad mapping control --------------------//
	{0x00, 1,{0x80}},             //panel pad mapping control
	{0xcc, 15,{0x29,0x2a,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x06,0x00,0x08,0x00,0x00,0x00,0x00}},

	{0x00, 1,{0x90}},             //panel pad mapping control
	{0xcc, 15,{0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x29,0x2a,0x09,0x0b,0x0d,0x0f,0x11,0x13}},

	{0x00, 1,{0xa0}},             //panel pad mapping control
	{0xcc, 14,{0x05,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00}},

  	{0x00, 1,{0xb0}},             //panel pad mapping control
	{0xcc, 15,{0x29,0x2a,0x13,0x11,0x0f,0x0d,0x0b,0x09,0x01,0x00,0x07,0x00,0x00,0x00,0x00}},

	{0x00, 1,{0xc0}},             //panel pad mapping control
	{0xcc, 15,{0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x29,0x2a,0x14,0x12,0x10,0x0e,0x0c,0x0a}},

	{0x00, 1,{0xd0}},             //panel pad mapping control
	{0xcc, 14,{0x02,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00}},

//-------------------- panel timing setting --------------------//
	{0x00, 1,{0x80}},             //panel VST setting
	{0xce, 12,{0x89,0x05,0x10,0x88,0x05,0x10,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1,{0x90}},             //panel VEND setting
	{0xce, 14,{0x54,0xfc,0x10,0x54,0xfd,0x10,0x55,0x00,0x10,0x55,0x01,0x10,0x00,0x00}},

	{0x00, 1,{0xa0}},             //panel CLKA1/2 setting
	{0xce, 14,{0x58,0x07,0x04,0xfc,0x00,0x10,0x00,0x58,0x06,0x04,0xfd,0x00,0x10,0x00}},

	{0x00, 1,{0xb0}},             //panel CLKA3/4 setting
	{0xce, 14,{0x58,0x05,0x04,0xfe,0x00,0x10,0x00,0x58,0x04,0x04,0xff,0x00,0x10,0x00}},

	{0x00, 1,{0xc0}},             //panel CLKb1/2 setting
	{0xce, 14,{0x58,0x03,0x05,0x00,0x00,0x10,0x00,0x58,0x02,0x05,0x01,0x00,0x10,0x00}},

	{0x00, 1,{0xd0}},             //panel CLKb3/4 setting
	{0xce, 14,{0x58,0x01,0x05,0x02,0x00,0x10,0x00,0x58,0x00,0x05,0x03,0x00,0x10,0x00}},

	{0x00, 1,{0x80}},             //panel CLKc1/2 setting
	{0xcf, 14,{0x50,0x00,0x05,0x04,0x00,0x10,0x00,0x50,0x01,0x05,0x05,0x00,0x10,0x00}},

	{0x00, 1,{0x90}},             //panel CLKc3/4 setting
	{0xcf, 14,{0x50,0x02,0x05,0x06,0x00,0x10,0x00,0x50,0x03,0x05,0x07,0x00,0x10,0x00}},

	{0x00, 1,{0xa0}},             //panel CLKd1/2 setting
	{0xcf, 14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1,{0xb0}},             //panel CLKd3/4 setting
	{0xcf, 14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
 
	{0x00, 1,{0xc0}},             //panel ECLK setting
	{0xcf, 11,{0x39,0x39,0x20,0x20,0x00,0x00,0x01,0x01,0x20,0x00,0x00}},

	{0x00, 1,{0xb5}},
	{0xc5, 6,{0x0b,0x95,0xff,0x0b,0x95,0xff}},

	{0x00, 1,{0x00}},
	{0xE1, 20,{0x02,0x43,0x4E,0x5C,0x6A,0x77,0x77,0x9E,0x8B,0xA2,0x62,0x4F,0x62,0x40,0x41,0x34,0x27,0x19,0x0C,0x02}},
				// 255  253  251  248 244  239  231   203 175   143  112  80   52   24   16   11    7    4    2   0						
	{0x00, 1,{0x00}},
	{0xE2, 20,{0x02,0x43,0x4D,0x5C,0x6A,0x77,0x77,0x9E,0x8B,0xA2,0x62,0x4F,0x62,0x40,0x41,0x34,0x27,0x19,0x0C,0x02}},



////////////ian add start 1/////////////////////

	{0x00, 1,{0xa0}},             //VDD_18V=1.7V, LVDSVDD=1.6V
	{0xc1, 1,{0x02}},

	{0x00, 1,{0x80}},             //VDD_18V=1.7V, LVDSVDD=1.6V
	{0xc4, 1,{0x01}},

	{0x00, 1,{0xc2}},             //LVD voltage level setting
	{0xf5, 1,{0xc0}},

	{0x00, 1,{0x88}},		//chopper
	{0xC4, 1,{0x80}},

	{0x35, 1,{0x00}},

/////////////ian add edd 1///////////////////
                                
	// Note
	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.
};


#if 1
static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 0, {0x00}},
    {REGFLAG_DELAY, 150, {}},

    // Display ON
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
	{0x28, 0, {0x00}},

    // Sleep Mode On
	{0x10, 0, {0x00}},

	{REGFLAG_DELAY, 150, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;
	lcm_debug("%s %d\n", __func__,__LINE__);
    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}



/* --------------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------------- */

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	lcm_debug("%s %d\n", __func__,__LINE__);
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	  	lcm_debug("%s %d\n", __func__,__LINE__);
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#ifdef LCM_DSI_CMD_MODE
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = BURST_VDO_MODE;
#endif
	

		params->dsi.esd_check_enable = 1;
		params->dsi.customization_esd_check_enable = 1;
		params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
		params->dsi.lcm_esd_check_table[0].count = 1;
		params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;

		/////////////////ian add start 2//////////////////////////////////////
		params->dsi.lcm_esd_check_table[1].cmd = 0xac;
		params->dsi.lcm_esd_check_table[1].count = 1;
		params->dsi.lcm_esd_check_table[1].para_list[0] = 0x00;

		params->dsi.lcm_esd_check_table[2].cmd = 0x0d;
		params->dsi.lcm_esd_check_table[2].count = 1;
		params->dsi.lcm_esd_check_table[2].para_list[0] = 0x00;		
		/////////////////ian add end 2//////////////////////////////////////
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 4;//4;//2;
		params->dsi.vertical_backporch					= 14;//16;//14;
		params->dsi.vertical_frontporch					= 16;//16;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 4;//6;//2;
		params->dsi.horizontal_backporch				= 34;//44;//44;//42;
		params->dsi.horizontal_frontporch				= 24;//44;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.PLL_CLOCK = 190;//dsi clock customization: should config clock value directly

}

static unsigned int lcm_compare_id(void)
{
	int data_array[4]; 
    char buffer[5]; 
    char id_high=0; 
    char id_low=0; 
    int id=0; 
    int lcm_adc = 0, data[4] = {0,0,0,0};

    IMM_GetOneChannelValue(12,data,&lcm_adc);
	lcm_debug("otm1284a_dsbj_lcm_compare_id kernel -- lcm_adc = %d \n",lcm_adc);
	lcm_debug("%s %d\n", __func__,__LINE__);

	disp_gpio_set(DISP_ENP,1);
	MDELAY(15);
	disp_gpio_set(DISP_ENN,1);
	MDELAY(15);

	SET_RESET_PIN(1);
	MDELAY(10);
    SET_RESET_PIN(0);
	MDELAY(10);
    SET_RESET_PIN(1);
	MDELAY(10);

    data_array[0] = 0x00053700; 
    dsi_set_cmdq(data_array, 1, 1); 
    read_reg_v2(0xa1, buffer, 5); 

    id_high = buffer[2]; 
    id_low = buffer[3]; 
    id = (id_high<<8) | id_low; 

    #ifdef BUILD_LK 
                printf("%s, LK OTM1284 debug: OTM1284 id = 0x%08x\n", __func__, id); 
    #else 
                printk("%s, kernel OTM1284 horse debug: OTM1284 id = 0x%08x\n", __func__, id); 
    #endif 
	 return ((LCM_ID_OTM1284A_DSBJ == id)&&(lcm_adc>100))?1:0;
}


static void lcm_init(void)
{
#if 1//chenjun
	unsigned char cmd = 0x0;
	unsigned char data = 0xFF;
	int ret=0;
	cmd=0x00;
	data=0x0E;

	//set_gpio_lcd_enp(1);
	disp_gpio_set(DISP_ENP,1);
	MDELAY(15);
	//set_gpio_lcd_enn(1);
	disp_gpio_set(DISP_ENN,1);
	MDELAY(15);
#ifdef BUILD_LK
	ret=TPS65132_write_byte(cmd,data);
    if(ret)    	
    dprintf(0, "[LK]otm1284a_dsbj----tps6132----cmd=%0x--i2c write error----\n",cmd);    	
	else
	dprintf(0, "[LK]otm1284a_dsbj----tps6132----cmd=%0x--i2c write success----\n",cmd);    		
#else
	ret=tps65132_write_bytes(cmd,data);
	if(ret<0)
	printk("[KERNEL]otm1284a_dsbj----tps6132---cmd=%0x-- i2c write error-----\n",cmd);
	else
	printk("[KERNEL]otm1284a_dsbj----tps6132---cmd=%0x-- i2c write success-----\n",cmd);
#endif
	
	cmd=0x01;
	data=0x0E;
#ifdef BUILD_LK
	ret=TPS65132_write_byte(cmd,data);
    if(ret)    	
	    dprintf(0, "[LK]otm1284a_dsbj----tps6132----cmd=%0x--i2c write error----\n",cmd);    	
	else
		dprintf(0, "[LK]otm1284a_dsbj----tps6132----cmd=%0x--i2c write success----\n",cmd);   
#else
	ret=tps65132_write_bytes(cmd,data);
	if(ret<0)
	printk("[KERNEL]otm1284a_dsbj----tps6132---cmd=%0x-- i2c write error-----\n",cmd);
	else
	printk("[KERNEL]otm1284a_dsbj----tps6132---cmd=%0x-- i2c write success-----\n",cmd);
#endif
#endif
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(10);

	push_table(lcm_initialization_setting,
		   sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{

	push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
	MDELAY(10);

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);


#if 1//chenjun
	disp_gpio_set(DISP_ENN,0);
	MDELAY(50);
	disp_gpio_set(DISP_ENP,0);
	MDELAY(150);
#endif

#ifdef BUILD_LK
	printf("uboot %s\n", __func__);
#else
	pr_debug("kernel %s\n", __func__);
#endif
}


static void lcm_resume(void)
{
#ifdef BUILD_LK
	printf("uboot %s\n", __func__);
#else
	pr_debug("kernel %s\n", __func__);
#endif
/* push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1); */
	lcm_init();
}

#ifdef LCM_DSI_CMD_MODE
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
	unsigned char x0_LSB = (x0 & 0xFF);
	unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
	unsigned char x1_LSB = (x1 & 0xFF);
	unsigned char y0_MSB = ((y0 >> 8) & 0xFF);
	unsigned char y0_LSB = (y0 & 0xFF);
	unsigned char y1_MSB = ((y1 >> 8) & 0xFF);
	unsigned char y1_LSB = (y1 & 0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00000000;
	data_array[4]= 0x00053902;
	data_array[5]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[6]= (y1_LSB);
	data_array[7]= 0x00000000;
	data_array[8]= 0x002c3909;

	dsi_set_cmdq(&data_array, 9, 0);

}
#endif


LCM_DRIVER otm1284a_dsbj_hd720_dsi_vdo_tm_lcm_drv = 
{
    .name			= "otm1284a_dsbj_dsi",
	.set_util_funcs = lcm_set_util_funcs,
	.compare_id = lcm_compare_id,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
#if defined(LCM_DSI_CMD_MODE)
	.update = lcm_update,
#endif
};
