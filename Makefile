IDIR =../include
CC=gcc
CFLAGS =I$(IDIR)

ODIR = obj 
LDIR = ../include

LIBS = -lm

_DEPS = camera.h camera_extern.h camera_iface.h datavis.h datavis_extern.h datavis_iface.h housekeeping.h housekeeping_extern.h housekeeping_iface.h macros.h main.h modules.h
DEPS = $(patsubst %, $(IDIR)/%,$(_DEPS))

_OBJ =main.o datavis.o camera.o housekeeping.o
OBJ = $(patsubst %, $(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
  $(CC) -c -o $@ $< $(CFLAGS)

atikcameramake: $(OBJ)
  $(CC) -o $@ $^ $(CFLAGS) $(LIBS)
  
.PHONY: clean

clean: 
  rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
  
  

%.o: %.c $(DEPS)
  $(CC) -c -o $@ $< $(CFLAGS)

atikcameramake: $(OBJ)
  $(CC) -o $@ $^ $(CFLAGS)
  
  
  
  
  
