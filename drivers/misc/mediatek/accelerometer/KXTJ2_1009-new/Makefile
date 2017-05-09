ccflags-y += -I$(srctree)/drivers/misc/mediatek/accelerometer/inc
ccflags-y += -I$(srctree)/drivers/misc/mediatek/hwmon/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/mach/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sensorHub/inc

ifeq ($(HQ_PROJECT_NAME),al910_ap)
		obj-y				+= kxtj2_1009_al910_ap.o
	else
		obj-y				+= kxtj2_1009.o
endif
