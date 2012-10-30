CPP=g++
CPPFLAGS=-I.  -I/usr/include/mysql -I/usr/local/include/mysql++ -I/usr/include/mysql++ -I/usr/include/curl -L/usr/lib/mysql -L/usr/local/lib -lmysqlpp -lmysqlclient  -lGeoIP -lcurl -lnsl -lz -lm -g -O0 #-I. -O2
CPPOPTS=$(CPP) $(CPPFLAGS)
COMPILE=$(CPPOPTS) -c

OBJS=sha1.o compat.o sendrecv.o send_control.o hashtree.o bin64.o bins.o channel.o datagram.o transfer.o httpgw.o ext/selector.o ext/seq_picker.o ext/simple_selector.o ext/BitSwiftSelector.o
OUT_DIR=bin
OUT_OBJS := $(addprefix $(OUT_DIR)/,$(OBJS))

$(OUT_DIR)/%.o: %.cpp
				$(COMPILE) -o $@ $<

all: swift

swift:	$(OUT_DIR)/swift.o $(OUT_OBJS)
		$(CPPOPTS) $(OUT_OBJS) $(OUT_DIR)/swift.o -o $(OUT_DIR)/swift
		
clean:	
	\rm  $(OUT_DIR)/*.o $(OUT_DIR)/swift  $(OUT_DIR)/ext/*.o
