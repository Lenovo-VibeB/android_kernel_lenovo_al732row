#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <string.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
//#include <mach/mt_gpio.h>
//#include <linux/xlog.h>
#endif

// ---------------------------------------------------------------------------
//  Local Constantsq
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (854)

#define REGFLAG_DELAY             							0XFFE
#define REGFLAG_END_OF_TABLE      							0xFFF   // END OF REGISTERS MARKER
#define LCM_ID_ILI9806E 										(0x980604)
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util;
unsigned int  g_lcmSuspend = 0;


#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

#define   LCM_DSI_CMD_MODE							(0)



struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[128];
};

extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
							
static struct LCM_setting_table lcm_initialization_setting[] = {
//JC
//0420 modify
{0xFF,5,{0xFF,0x98,0x06,0x04,0x01}},
{0x08,1,{0x10}},
{0x21,1,{0x01}},
{0x30,1,{0x01}},
{0x31,1,{0x02}},
{0x40,1,{0x10}},
{0x41,1,{0x33}},
{0x42,1,{0x03}},
{0x43,1,{0x09}},
{0x44,1,{0x07}},
{0x50,1,{0x78}},
{0x51,1,{0x78}},
{0x52,1,{0x00}},
{0x53,1,{0x40}},
{0x60,1,{0x07}},
{0x61,1,{0x00}},
{0x62,1,{0x08}},
{0x63,1,{0x00}},

{0xA0,1,{0x00}},
{0xA1,1,{0x08}},
{0xA2,1,{0x11}},
{0xA3,1,{0x0f}},
{0xA4,1,{0x09}},
{0xA5,1,{0x15}},
{0xA6,1,{0x08}},
{0xA7,1,{0x07}},
{0xA8,1,{0x04}},
{0xA9,1,{0x09}},
{0xAA,1,{0x0c}},
{0xAB,1,{0x05}},
{0xAC,1,{0x0c}},
{0xAD,1,{0x2d}},
{0xAE,1,{0x27}},
{0xAF,1,{0x00}},

{0xC0,1,{0x00}},
{0xC1,1,{0x08}},
{0xC2,1,{0x11}},
{0xC3,1,{0x10}},
{0xC4,1,{0x09}},
{0xC5,1,{0x15}},
{0xC6,1,{0x0a}},
{0xC7,1,{0x09}},
{0xC8,1,{0x03}},
{0xC9,1,{0x07}},
{0xCA,1,{0x03}},
{0xCB,1,{0x04}},
{0xCC,1,{0x0b}},
{0xCD,1,{0x2b}},
{0xCE,1,{0x27}},
{0xCF,1,{0x00}},



{0xFF,5,{0xFF,0x98,0x06,0x04,0x06}},
{0x00,1,{0x21}},
{0x01,1,{0x0A}},
{0x02,1,{0x00}},
{0x03,1,{0x00}},
{0x04,1,{0x01}},
{0x05,1,{0x01}},
{0x06,1,{0x80}},
{0x07,1,{0x06}},
{0x08,1,{0x01}},
{0x09,1,{0x80}},
{0x0A,1,{0x00}},
{0x0B,1,{0x00}},
{0x0C,1,{0x0A}},
{0x0D,1,{0x0A}},
{0x0E,1,{0x00}},
{0x0F,1,{0x00}},
{0x10,1,{0xF0}},
{0x11,1,{0xF4}},
{0x12,1,{0x04}},
{0x13,1,{0x00}},
{0x14,1,{0x00}},
{0x15,1,{0xC0}},
{0x16,1,{0x08}},
{0x17,1,{0x00}},
{0x18,1,{0x00}},
{0x19,1,{0x00}},
{0x1A,1,{0x00}},
{0x1B,1,{0x00}},
{0x1C,1,{0x00}},
{0x1D,1,{0x00}},
{0x20,1,{0x01}},
{0x21,1,{0x23}},
{0x22,1,{0x45}},
{0x23,1,{0x67}},
{0x24,1,{0x01}},
{0x25,1,{0x23}},
{0x26,1,{0x45}},
{0x27,1,{0x67}},
{0x30,1,{0x01}},
{0x31,1,{0x11}},
{0x32,1,{0x00}},
{0x33,1,{0xEE}},
{0x34,1,{0xFF}},
{0x35,1,{0xBB}},
{0x36,1,{0xCA}},
{0x37,1,{0xDD}},
{0x38,1,{0xAC}},
{0x39,1,{0x76}},
{0x3A,1,{0x67}},
{0x3B,1,{0x22}},
{0x3C,1,{0x22}},
{0x3D,1,{0x22}},
{0x3E,1,{0x22}},
{0x3F,1,{0x22}},
{0x40,1,{0x22}},
{0x52,1,{0x10}},
{0x53,1,{0x10}},
//{0x58,1,{0x97}},

{0xFF,5,{0xFF,0x98,0x06,0x04,0x07}},
{0x17,1,{0x22}},
{0x02,1,{0x77}},
{0xE1,1,{0x79}},
{0x18,1,{0x1d}},
{0x26,1,{0xb2}},
{0x06,1,{0x13}},

{0xFF,5,{0xFF,0x98,0x06,0x04,0x00}},
	{0x11,1,{0x00}},  // Sleep-Out
	{REGFLAG_DELAY, 120, {0}},	
	{0x29,1,{0x00}},  // Display On
	{REGFLAG_DELAY, 10, {0}},	  	 

};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 10, {}},
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 10, {}},
    // Sleep Mode On
	{0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


//static struct LCM_setting_table lcm_backlight_level_setting[] = {
	//{0x51, 1, {0xFF}},
	//{REGFLAG_END_OF_TABLE, 0x00, {}}
//};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++)
    {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd)
        {
			
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
static void init_lcm_registers(void)
{
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS * util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS * params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	params->physical_width=56;
	params->physical_height=99;
	
		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
    params->dsi.mode = SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; SYNC_PULSE_VDO_MODE SYNC_EVENT_VDO_MODE
 
 
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
 #if 1 	
	params->dsi.lcm_esd_check_table[1].cmd = 0x0d;
	params->dsi.lcm_esd_check_table[1].count = 1;
	params->dsi.lcm_esd_check_table[1].para_list[0] = 0x00;
#endif 
	
    // DSI
    /* Command mode setting */
    params->dsi.LANE_NUM = LCM_TWO_LANE;
    //The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;
 
		params->dsi.packet_size=256;
		
//yinyapeng add to disable ssc ==>
	params->dsi.ssc_disable = 1;
//<==end

		// Video mode setting		
    params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    
    params->dsi.word_count=FRAME_WIDTH*3;	//DSI CMD mode need set these two bellow params, different to 6577
    params->dsi.vertical_active_line=FRAME_HEIGHT;
    params->dsi.vertical_sync_active = 6;
    params->dsi.vertical_backporch = 14;//8;
    params->dsi.vertical_frontporch = 20;//8;
    params->dsi.vertical_active_line = FRAME_HEIGHT;
    params->dsi.horizontal_sync_active = 10;//8;
    params->dsi.horizontal_backporch = 80;//60;
    params->dsi.horizontal_frontporch = 80;//140;	
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;	
/*
		params->dsi.vertical_sync_active				= 1;// 3    2
		params->dsi.vertical_backporch					= 1;// 20   1
		params->dsi.vertical_frontporch					= 2; // 1  12
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 10;// 50  2
		params->dsi.horizontal_backporch				= 42;
		params->dsi.horizontal_frontporch				= 52;
		params->dsi.horizontal_bllp				= 85;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.compatibility_for_nvk = 0;*/

    	params->dsi.PLL_CLOCK = 210;//dsi clock customization: should config clock value directly
	}



static void lcm_suspend(void)
{

	push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
	/*
	SET_RESET_PIN(1);	
	MDELAY(10);	
	SET_RESET_PIN(0);
	*/
	//if (g_lcmSuspend != true)
	    g_lcmSuspend = 1;

	//printk("yinyapeng add g_lcmSuspend = %d in suspend",g_lcmSuspend);
}



#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
		       unsigned int width, unsigned int height)
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

	data_array[0] = 0x00053902;
	data_array[1] =
	    (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00053902;
	data_array[1] =
	    (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2b;
	data_array[2] = (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}
#endif
extern void DSI_clk_HS_mode(char enter);
static unsigned int lcm_compare_id(void)
{
		int array[4];
        char buffer[5];
        char id_high=0;
        char id_midd=0;
        char id_low=0;
        int id=0;
        int lcm_adc = 0, data[4] = {0,0,0,0};
        //Do reset here
        SET_RESET_PIN(1);
        SET_RESET_PIN(0);
        MDELAY(25);       
        SET_RESET_PIN(1);
        MDELAY(50);      
       
        array[0]=0x00063902;
        array[1]=0x0698ffff;
        array[2]=0x00000104;
        dsi_set_cmdq(array, 3, 1);
        MDELAY(10);
 
        array[0]=0x00033700;
        dsi_set_cmdq(array, 1, 1);
        //read_reg_v2(0x04, buffer, 3);//if read 0x04,should get 0x008000,that is both OK.
    
        read_reg_v2(0x00, buffer,1);
        id_high = buffer[0]; ///////////////////////0x98
 
        read_reg_v2(0x01, buffer,1);
        id_midd = buffer[0]; ///////////////////////0x06
 
        read_reg_v2(0x02, buffer,1);
        id_low = buffer[0]; ////////////////////////0x04
 
        id =(id_high << 16) | (id_midd << 8) | id_low;
        IMM_GetOneChannelValue(12,data,&lcm_adc); //read lcd _id
#if defined(BUILD_LK)
			  printf("ili9806e_hlt_hd720_dsi lk -- ili9806e 0x%x , 0x%x , 0x%x, 0x%x \n", id_high, id_midd, id_low, id);
#else
			  printk("ili9806e_hlt_hd720_dsi kernel -- ili9806e 0x%x , 0x%x , 0x%x, 0x%x \n", id_high, id_midd, id_low, id);
#endif

        if((LCM_ID_ILI9806E == id) && (lcm_adc > 3000))
		return 1;
	else
		return 0;
}
static void lcm_init(void)
{
#if 0
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(100);
#endif
	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	
	
	init_lcm_registers();
}
static void lcm_resume(void)
{
	//lcm_init();

	printk("yinyapeng add lcm_resume\n");	
	//if (g_lcmSuspend != false)
	g_lcmSuspend = 0;
	
	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER ili9806e_hlt_hd720_dsi_vdo_fwvga_lcm_drv = {
	.name = "ili9806e_hlt_hd720_dsi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	.compare_id = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
	.set_backlight	= lcm_setbacklight,
    .update         = lcm_update,
#endif
};

EXPORT_SYMBOL(g_lcmSuspend);

