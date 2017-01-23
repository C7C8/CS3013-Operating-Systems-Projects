#This file isn't modified from the one in the PDF; this comment
#not included, it's completely identical because I wget'd
#the URL provided.
#
# --Christopher Myers

obj-m := hello.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
