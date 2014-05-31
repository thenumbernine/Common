# the base of the build system -- everything includes this first

COMMON_PATH:=$(dir $(lastword $(MAKEFILE_LIST)))

buildVar = \
	$($(1)_$(PLATFORM))\
	$($(1)_$(BUILD))\
	$($(1)_$(DIST_TYPE))\
	$($(1)_$(PLATFORM)_$(DIST_TYPE))\
	$($(1)_$(DIST_TYPE)_$(BUILD))\
	$($(1)_$(PLATFORM)_$(BUILD))\
	$($(1)_$(PLATFORM)_$(DIST_TYPE)_$(BUILD))

space :=
space +=
concat = $(subst $(space),,$(strip $1))

SRCDIR_BASE=src
SOURCES=$(shell find $(SRCDIR_BASE) -type f -name *.cpp)
HEADERS=$(shell find include -type f)
OBJECTS=$(patsubst $(SRCDIR_BASE)/%.cpp, %.o, $(SOURCES))

OBJDIR_BASE=obj
OBJDIR=$(OBJDIR_BASE)/$(PLATFORM)/$(BUILD)
OBJPATHS=$(addprefix $(OBJDIR)/, $(OBJECTS))

INCLUDE=include
INCLUDE+=$(COMMON_PATH)include

MACROS=PLATFORM_$(PLATFORM) BUILD_$(BUILD)
MACROS_debug=DEBUG
MACROS_release=NDEBUG

CC=clang++
CFLAGS=-c -Wall -std=c++11
CFLAGS_debug=-O0 -mfix-and-continue -gdwarf-2
CFLAGS_release=-O3

#LIBPATHS
#LIBS

LD=clang++
LDFLAGS_lib=-dynamiclib -undefined suppress -flat_namespace

.PHONY: default
default: all

.PHONY: all
all: osx

.PHONY: help
help:
	echo "make <platform>"
	echo "platform: osx"

.PHONY: osx
osx:
	$(MAKE) PLATFORM="osx" build_platform

.PHONY: build_platform
build_platform: $(PLATFORM)_debug $(PLATFORM)_release

.PHONY: $(PLATFORM)_debug
$(PLATFORM)_debug:
	$(MAKE) BUILD="debug" builddist

.PHONY: $(PLATFORM)_release
$(PLATFORM)_release:
	$(MAKE) BUILD="release" builddist

DISTDIR_BASE=dist
DISTDIR=$(DISTDIR_BASE)/$(PLATFORM)/$(BUILD)
DIST_PREFIX_osx_lib=lib
DIST_SUFFIX_osx_lib=.dylib
DIST=$(call concat,$(DISTDIR)/$(call buildVar,DIST_PREFIX)$(DIST_FILENAME)$(call buildVar,DIST_SUFFIX))

.PHONY: builddist
builddist: CFLAGS+= $(call buildVar,CFLAGS)
builddist: CFLAGS+= $(addprefix -I,$(INCLUDE) $(call buildVar,INCLUDE))
builddist: CFLAGS+= $(addprefix -D,$(MACROS) $(call buildVar,MACROS))
builddist: LDFLAGS+= $(call buildVar,LDFLAGS)
builddist: LDFLAGS+= $(addprefix -l,$(LIBS) $(call buildVar,LIBS))
builddist: LDFLAGS+= $(addprefix -L,$(LIBPATHS) $(call buildVar,LIBPATHS))
builddist: $(DIST)
builddist: 
	@echo done with $(DIST)
	@echo

$(OBJDIR)/%.o : $(SRCDIR_BASE)/%.cpp $(HEADERS)
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

$(DIST):: $(OBJPATHS)
	-mkdir -p $(@D)
	$(LD) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	-rm -fr $(OBJDIR_BASE)

.PHONY: distclean
distclean:
	-rm -fr $(DISTDIR_BASE)

