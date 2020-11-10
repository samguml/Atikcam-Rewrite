CC=g++
RM= /bin/rm -vf

EDCFLAGS:=$(CFLAGS)
EDLDFLAGS:=$(LDFLAGS)

EDCFLAGS:= -Wall -fno-strict-aliasing -std=gnu11 -O2 $(EDCFLAGS)
EDLDFLAGS:= -lm -lpthread $(EDLDFLAGS)

TARGETOBJS= src/main.o src/datavis.o src/camera.o src/housekeeping.o    

TARGET = ATIKCAM-REWRITE-1.out

all: build/$(TARGET)

build:
	mkdir build

build/$(TARGET): $(TARGETOBJS) build
	$(CC) $(TARGETOBJS) $(LINKOPTIONS) -o $@ \
	$(EDLDFLAGS)

%.o: %.c
	$(CC) $(EDCFLAGS) -Iinclude/ -o $@ -c $<

clean:
	$(RM) build/$(TARGET)
	$(RM) $(TARGETOBJS)
