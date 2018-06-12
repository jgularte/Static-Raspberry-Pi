
CC = gcc
ODIR = obj
SDIR = src
DOCDIR = doc
INC = -Iinclude
LIBS = -lpthread -lwiringPi -lm
CFLAGS = -Wall
BINS = diagpov pov

_OBJ_DIAP = diagpov.o motor.o utils.o web_client.o
_OBJ_POV = pov.o motor.o utils.o web_client.o


OBJS_DIAP = $(patsubst %,$(ODIR)/%,$(_OBJ_DIAP))
OBJS_POV = $(patsubst %,$(ODIR)/%,$(_OBJ_POV))

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) $(LIBS)

diagpov: $(OBJS_DIAP)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

pov: $(OBJS_POV)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all: $(BINS)

.PHONY: doc
doc:
	doxygen

clean:
	rm -Rf $(ODIR) $(BINS) $(DOCDIR)
