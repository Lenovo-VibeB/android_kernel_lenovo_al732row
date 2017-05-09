#define pr_fmt(fmt) "["KBUILD_MODNAME"] " fmt
#include <linux/io.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/kfifo.h>

#include <linux/firmware.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/of.h>

#include <mt-plat/mt_boot.h>

unsigned int g_meta_com_type = META_UNKNOWN_COM;
unsigned int g_meta_com_id = 0;
unsigned int g_meta_uart_port = 0;

static struct platform_driver meta_com_type_info = {
	.driver = {
		   .name = "meta_com_type_info",
		   .bus = &platform_bus_type,
		   .owner = THIS_MODULE,
		   },
	.id_table = NULL,
};

static struct platform_driver meta_com_id_info = {
	.driver = {
		   .name = "meta_com_id_info",
		   .bus = &platform_bus_type,
		   .owner = THIS_MODULE,
		   },
	.id_table = NULL,
};

static struct platform_driver meta_uart_port_info = {
	.driver = {
		   .name = "meta_uart_port_info",
		   .bus = &platform_bus_type,
		   .owner = THIS_MODULE,
		   },
	.id_table = NULL,
};



#ifdef CONFIG_OF
struct boot_tag_meta_com {
	u32 size;
	u32 tag;
	u32 meta_com_type;	/* identify meta via uart or usb */
	u32 meta_com_id;	/* multiple meta need to know com port id */
	u32 meta_uart_port;	/* identify meta uart port number */
};
#endif

/* usb android will check whether is com port enabled default.
   in normal boot it is default enabled. */
bool com_is_enable(void)
{
	if (get_boot_mode() == NORMAL_BOOT)
		return false;
	else
		return true;
}

/*[FACTORY_TEST_BY_APK] modify start*/
unsigned int get_boot_mode_ex(void)
{
    return g_boot_mode_ex;
}
/*[FACTORY_TEST_BY_APK] modify end*/
unsigned int get_meta_com_type(void)
{
	return g_meta_com_type;
}

unsigned int get_meta_com_id(void)
{
	return g_meta_com_id;
}

unsigned int get_meta_uart_port(void)
{
	return g_meta_uart_port;
}


static ssize_t meta_com_type_show(struct device_driver *driver, char *buf)
{
	return sprintf(buf, "%d\n", g_meta_com_type);
}

static ssize_t meta_com_type_store(struct device_driver *driver, const char *buf, size_t count)
{
	/*Do nothing */
	return count;
}

DRIVER_ATTR(meta_com_type_info, 0644, meta_com_type_show, meta_com_type_store);


static ssize_t meta_com_id_show(struct device_driver *driver, char *buf)
{
	return sprintf(buf, "%d\n", g_meta_com_id);
}

static ssize_t meta_com_id_store(struct device_driver *driver, const char *buf, size_t count)
{
	/*Do nothing */
	return count;
}

DRIVER_ATTR(meta_com_id_info, 0644, meta_com_id_show, meta_com_id_store);
static ssize_t meta_uart_port_show(struct device_driver *driver, char *buf)
{
	return sprintf(buf, "%d\n", g_meta_uart_port);
}

static ssize_t meta_uart_port_store(struct device_driver *driver, const char *buf, size_t count)
{
	/*Do nothing */
	return count;
}

DRIVER_ATTR(meta_uart_port_info, 0644, meta_uart_port_show, meta_uart_port_store);


static int __init create_sysfs(void)
{
	int ret;
	enum boot_mode_t bm = get_boot_mode();

#ifdef CONFIG_OF
	if (of_chosen) {
		struct boot_tag_meta_com *tags;

		tags = (struct boot_tag_meta_com *)of_get_property(of_chosen, "atag,meta", NULL);
		if (tags) {
			g_meta_com_type = tags->meta_com_type;
			g_meta_com_id = tags->meta_com_id;
			g_meta_uart_port = tags->meta_uart_port;
			pr_debug
			    ("[%s] g_meta_com_type = %d, g_meta_com_id = %d, g_meta_uart_port=%d.\n",
			     __func__, g_meta_com_type, g_meta_com_id, g_meta_uart_port);
		} else
			pr_warn("[%s] No atag,meta found !\n", __func__);
	} else
		pr_warn("[%s] of_chosen is NULL !\n", __func__);
#endif

	if (bm == META_BOOT || bm == ADVMETA_BOOT || bm == ATE_FACTORY_BOOT || bm == FACTORY_BOOT) {
		/* register driver and create sysfs files */
		ret = driver_register(&meta_com_type_info.driver);
		if (ret)
			pr_warn("fail to register META COM TYPE driver\n");
		ret =
		    driver_create_file(&meta_com_type_info.driver, &driver_attr_meta_com_type_info);
		if (ret)
			pr_warn("fail to create META COM TPYE sysfs file\n");

		ret = driver_register(&meta_com_id_info.driver);
		if (ret)
			pr_warn("fail to register META COM ID driver\n");
		ret = driver_create_file(&meta_com_id_info.driver, &driver_attr_meta_com_id_info);
		if (ret)
			pr_warn("fail to create META COM ID sysfs file\n");
		ret = driver_register(&meta_uart_port_info.driver);
		if (ret)
			pr_warn("fail to register META UART PORT driver\n");
		ret =
		    driver_create_file(&meta_uart_port_info.driver,
				       &driver_attr_meta_uart_port_info);
		if (ret)
			pr_warn("fail to create META UART PORT sysfs file\n");
	}

	return 0;
}

static int __init boot_mod_init(void)
{
	create_sysfs();
	return 0;
}

static void __exit boot_mod_exit(void)
{
}


#if 0
/* NEW FEATURE: multi-boot mode */
static int get_boot_mode(void)
{
    int fd;
    size_t s;
    char boot_mode[4] = {'0'};

    fd = open("/sys/class/BOOT/BOOT/boot/boot_mode", O_RDONLY);
    if (fd < 0) {
        ERROR("fail to open: %s\n", "/sys/class/BOOT/BOOT/boot/boot_mode");
        return 0;
    }

    s = read(fd, (void *)&boot_mode, sizeof(boot_mode) - 1);
    close(fd);

    if (s <= 0) {
        ERROR("could not read boot mode sys file\n");
        return 0;
    }
    /*[FACTORY_TEST_BY_APK] modify start*/
    printf("system core init get_boot_mode boot_mode[0]:%d, boot_mode[1]:%d; atoi(&boot_mode):%d\n",boot_mode[0],boot_mode[1],atoi((const char*)boot_mode));
    if(boot_mode[1] == 49) //liunianliang_add 20160107,APK test mode bootup
       return atoi((const char*)boot_mode);
    else
    /*[FACTORY_TEST_BY_APK] modify end*/
    boot_mode[s] = '\0';
    return atoi((const char*)boot_mode);
}
 #endif


module_init(boot_mod_init);
module_exit(boot_mod_exit);
MODULE_DESCRIPTION("MTK Boot Information Querying Driver");
MODULE_LICENSE("GPL");

/*[FACTORY_TEST_BY_APK] modify start*/
EXPORT_SYMBOL(get_boot_mode_ex);//Huangyisong_add 20130823
/*[FACTORY_TEST_BY_APK] modify end*/
