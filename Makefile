CC  ?= gcc
CXX ?= g++

LIBS = -lz -lm -lbz2 -llzma -lpthread -lcurl 

CRYPTO_TRY=$(shell echo 'int main(){}'|$(CXX) -x c++ - -lcrypto 2>/dev/null -o /dev/null; echo $$?)
ifeq "$(CRYPTO_TRY)" "0"
$(info Crypto library is available to link; adding -lcrypto to LIBS)
LIBS += -lcrypto
else
$(info Crypto library is not available to link; will not use -lcrypto)
endif


#if htslib source is defined
ifdef HTSSRC

#if hts source is set to systemwide
ifeq ($(HTSSRC),systemwide)
$(info HTSSRC set to systemwide; assuming systemwide installation)
LIBS += -lhts

else

#if hts source path is given
# Adjust $(HTSSRC) to point to your top-level htslib directory
$(info HTSSRC defined: $(HTSSRC))
CPPFLAGS += -I"$(realpath $(HTSSRC))"
LIBHTS := $(HTSSRC)/libhts.a
LIBS := $(LIBHTS) $(LIBS)

endif

#if htssrc not defined
else

$(info HTSSRC not defined; using htslib submodule)
$(info Use `make HTSSRC=/path/to/htslib` to build metadamage using a local htslib installation)
$(info Use `make HTSSRC=systemwide` to build metadamage using the systemwide htslib installation)


HTSSRC := $(CURDIR)/htslib
CPPFLAGS += -I$(HTSSRC)
LIBHTS := $(HTSSRC)/libhts.a
LIBS := $(LIBHTS) $(LIBS)

all: .activate_module

endif

.PHONY: .activate_module 

.activate_module:
	git submodule update --init --recursive
	$(MAKE) -C $(HTSSRC)



#modied from htslib makefile
FLAGS = -O3 
CPPFLAGS := $(filter-out -DNDEBUG,$(CPPFLAGS))
FLAGS2 = $(CPPFLAGS) $(FLAGS) $(LDFLAGS)

CFLAGS := $(FLAGS2) $(CFLAGS)
CXXFLAGS := $(FLAGS2) $(CXXFLAGS)

CSRC = $(wildcard *.c)
CXXSRC = $(wildcard *.cpp)
OBJ = $(CSRC:.c=.o) $(CXXSRC:.cpp=.o)

prefix      = /usr/local
exec_prefix = $(prefix)
bindir      = $(exec_prefix)/bin

INSTALL = install
INSTALL_DIR = $(INSTALL) -dm0755
INSTALL_PROGRAM = $(INSTALL) -m0755

PROGRAMS = metabeta

all: $(PROGRAMS) 

PACKAGE_VERSION  = 0.1

ifneq "$(wildcard .git)" ""
PACKAGE_VERSION := $(shell git describe --always --dirty)
version.h: $(if $(wildcard version.h),$(if $(findstring "$(PACKAGE_VERSION)",$(shell cat version.h)),,force))
endif

version.h:
	echo '#define METABETA_VERSION "$(PACKAGE_VERSION)"' > $@

.PHONY: all clean install install-all install-misc misc test

misc: 
	$(MAKE) -C misc HTSSRC="$(realpath $(HTSSRC))"

-include $(OBJ:.o=.d)

%.o: %.c
	$(CC) -c  $(CFLAGS) $*.c
	$(CC) -MM $(CFLAGS) $*.c >$*.d

%.o: %.cpp
	$(CXX) -c  $(CXXFLAGS) $*.cpp
	$(CXX) -MM $(CXXFLAGS) $*.cpp >$*.d


metabeta: version.h $(OBJ)
	$(CXX) $(FLAGS) -o metabeta *.o $(LIBS)

clean: 
	rm  -f *.o *.d $(PROGRAMS) version.h *~
	$(MAKE) -C misc clean

force:

install: all
	$(INSTALL_DIR) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) $(PROGRAMS) $(DESTDIR)$(bindir)
	$(MAKE) -C misc HTSSRC="$(realpath $(HTSSRC))" install

install-misc: misc
	$(MAKE) -C misc HTSSRC="$(realpath $(HTSSRC))" install-misc

install-all: install install-misc
