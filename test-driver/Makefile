# If KERNELRELEASE is defined, invoke from the kernel build system
ifneq ($(KERNELRELEASE),)
	obj-m := hello.o

# Otherwise call directly from the command line;
# then invoke the kernel build system
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif
