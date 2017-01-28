# This file was automatically generated by bakefile.
#
# Any manual changes will be lost if it is regenerated,
# modify the source .bkl file instead if possible.

# You may define standard make variables such as CFLAGS or
# CXXFLAGS to affect the build. For example, you could use:
#
#      make CXXFLAGS=-g
#
# to build with debug information. The full list of variables
# that can be used by this makefile is:
# AR, CC, CFLAGS, CPPFLAGS, CXX, CXXFLAGS, LD, LDFLAGS, MAKE, RANLIB.

# You may also specify config=Debug|Release
# or their corresponding lower case variants on make command line to select
# the corresponding default flags values.
ifeq ($(config),debug)
override config := Debug
endif
ifeq ($(config),release)
override config := Release
endif
ifeq ($(config),Debug)
CPPFLAGS ?= -DDEBUG
CFLAGS ?= -g -O0
CXXFLAGS ?= -g -O0
LDFLAGS ?= -g
else ifeq ($(config),Release)
CPPFLAGS ?= -DNDEBUG
CFLAGS ?= -O2
CXXFLAGS ?= -O2
else ifneq (,$(config))
$(warning Unknown configuration "$(config)")
endif

# Use "make RANLIB=''" for platforms without ranlib.
RANLIB ?= ranlib

CC := cc
CXX := c++

# The directory for the build files, may be overridden on make command line.
builddir = .

all: $(builddir)/dflektr.out

$(builddir)/dflektr: $(builddir)/dflektr_cAnsiUtil.o $(builddir)/dflektr_cCmdParse.o $(builddir)/dflektr_cDeflector.o $(builddir)/dflektr_cListener.o $(builddir)/dflektr_dflektr_main.o
	$(CXX) -o $@ $(LDFLAGS) $(builddir)/dflektr_cAnsiUtil.o $(builddir)/dflektr_cCmdParse.o $(builddir)/dflektr_cDeflector.o $(builddir)/dflektr_cListener.o $(builddir)/dflektr_dflektr_main.o -pthread

$(builddir)/dflektr_cAnsiUtil.o: dflektr/cAnsiUtil.cpp
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -MD -MP -pthread dflektr/cAnsiUtil.cpp

$(builddir)/dflektr_cCmdParse.o: dflektr/cCmdParse.cpp
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -MD -MP -pthread dflektr/cCmdParse.cpp

$(builddir)/dflektr_cDeflector.o: dflektr/cDeflector.cpp
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -MD -MP -pthread dflektr/cDeflector.cpp

$(builddir)/dflektr_cListener.o: dflektr/cListener.cpp
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -MD -MP -pthread dflektr/cListener.cpp

$(builddir)/dflektr_dflektr_main.o: dflektr/dflektr_main.cpp
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -MD -MP -pthread dflektr/dflektr_main.cpp

clean:
	rm -f *.o
	rm -f *.d
	rm -f $(builddir)/dflektr.out

.PHONY: all clean

# Dependencies tracking:
-include *.d
