# This Makefile is create next execute binary files.
#    1.self_com
#           


CFLAGS      = -Wall -O2 
#TARGET      = arm-none-linux-gnueabi-
TARGET      = arm-linux-gnueabi-
#TARGET	=
CC          = $(TARGET)gcc
STRIP       = $(TARGET)strip
INSTALL_DIR = /usr/local/bin
USERLIBS    = -lSerialFunc -lm

PRG_NAME1=self_com
PRG_NAME2=port2_com
PRG_NAME3=setrts_com
PRG_NAME4=getdsr_com

PROGS       = $(PRG_NAME1) $(PRG_NAME2) $(PRG_NAME3) $(PRG_NAME4)\


##### Make all######################
all : $(PROGS)


### Program build ###########################################################
$(PRG_NAME1) : $(PRG_NAME1).c $(USERLIBS)
	$(CC) $(CFLAGS) -o $(PRG_NAME1) $(PRG_NAME1).c \
		$(USERLIBS) 
	$(STRIP) $(PRG_NAME1)

$(PRG_NAME2) : $(PRG_NAME2).c $(USERLIBS)
	$(CC) $(CFLAGS) -o $(PRG_NAME2) $(PRG_NAME2).c \
		$(USERLIBS)
	$(STRIP) $(PRG_NAME2)

$(PRG_NAME3) : $(PRG_NAME3).c $(USERLIBS)
	$(CC) $(CFLAGS) -o $(PRG_NAME3) $(PRG_NAME3).c \
		$(USERLIBS)
	$(STRIP) $(PRG_NAME3)

$(PRG_NAME4) : $(PRG_NAME4).c $(USERLIBS)
	$(CC) $(CFLAGS) -o $(PRG_NAME4) $(PRG_NAME4).c \
		$(USERLIBS)
	$(STRIP) $(PRG_NAME4)

##### Make install #################
install :
	cp -a $(PROGS) $(INSTALL_DIR)
	chown root.root $(INSTALL_DIR)/$(PROGS)
	chown 4755 $(INSTALL_DIR)/$(PROGS)


##### Make uninstall ###############
uninstall :
	rm -f $(INSTALL_DIR)/$(PROGS)


##### Make clean ###################
clean:
	rm -f $(PROGS)

