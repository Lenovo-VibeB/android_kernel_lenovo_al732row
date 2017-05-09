
HQ_PROJECT_NAME ?= al732row

$(warning "CONFIG_TABLET_PROJECT:$(CONFIG_TABLET_PROJECT)")
ifeq ($(CONFIG_TABLET_PROJECT),y)
# is tablet project
obj-y				+= tb_kd_camera_hw.o
else
# not tablet project
	ifeq ($(HQ_PROJECT_NAME),al732row)
		obj-y				+= kd_camera_hw_732.o
	else
		obj-y				+= kd_camera_hw.o
    endif
endif
