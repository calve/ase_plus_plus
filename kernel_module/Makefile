# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
	obj-m := ase_cmd.o tracktime.o
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
  ifeq ($(USER), goudale) # Use relative paths on Arthur's computer
	KERNELDIR ?= ../../kvm/linux-4.0-rc4
	BUILDDIR = ../../kvm/build/kvm32
  else
	KERNELDIR ?= /gfs/franquenouille/linux-3.19.1
	BUILDDIR = /gfs/franquenouille/build/kvm32
  endif
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) O=$(BUILDDIR) M=$(PWD) modules
endif

