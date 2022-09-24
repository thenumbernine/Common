# the base of the build system -- everything includes this first

COMMON_PATH:=$(dir $(lastword $(MAKEFILE_LIST)))

#https://stackoverflow.com/questions/664601/in-gnu-make-how-do-i-convert-a-variable-to-lower-case
lowercase = $(shell echo $(1) | tr A-Z a-z)
uppercase = $(shell echo $(1) | tr a-z A-Z)

include $(COMMON_PATH)Config.mk

buildVar = \
	$($(1)_$(PLATFORM))\
	$($(1)_$(BUILD))\
	$($(1)_$(DIST_TYPE))\
	$($(1)_$(PLATFORM)_$(DIST_TYPE))\
	$($(1)_$(DIST_TYPE)_$(BUILD))\
	$($(1)_$(PLATFORM)_$(BUILD))\
	$($(1)_$(PLATFORM)_$(DIST_TYPE)_$(BUILD))

#I forget where I got the previous concat-without-spaces that seems to have broken compatability over the  years
# but the new one that fixes gnu make's broken concat compatability is here: https://stackoverflow.com/a/53124524
empty:=
space:=$(empty) $(empty)
concat = $(subst ${space},,$(strip $1))
escapechar :=\ 
escapechar :=$(subst $(space),,$(escapechar))

SRCDIR_BASE=src
SOURCES=$(shell $(FIND) $(SRCDIR_BASE) -type f -name *.cpp)
OBJECTS=$(patsubst $(SRCDIR_BASE)/%.cpp, %.o, $(SOURCES))

OBJDIR_BASE=obj
OBJDIR=$(OBJDIR_BASE)/$(PLATFORM)/$(BUILD)
OBJPATHS=$(addprefix $(OBJDIR)/, $(OBJECTS))

INCLUDE=include
INCLUDE+=$(COMMON_PATH)include
#HEADERS=$(shell $(FIND) $(INCLUDE) -type f)

MACROS=PLATFORM_$(call uppercase,$(PLATFORM)) BUILD_$(call uppercase,$(BUILD))
MACROS_debug=DEBUG
MACROS_release=NDEBUG

CPPVER=c++17

CFLAGS_osx=-c -Wall -std=$(CPPVER)
CFLAGS_osx_debug=-O0 -gdwarf-2 -mfix-and-continue
CFLAGS_osx_release=-O3
CFLAGS_lib_osx+=-fPIC
CFLAG_OUTPUT_osx=-o
CFLAG_INCLUDE_osx=-I
CFLAG_MACRO_osx=-D
LDFLAG_OUTPUT_osx=-o

CFLAGS_linux=-c -Wall -std=$(CPPVER) -fPIC
CFLAGS_linux_debug=-O0 -gdwarf-2
CFLAGS_linux_release=-O3
CFLAG_OUTPUT_linux=-o
CFLAG_INCLUDE_linux=-I
CFLAG_MACRO_linux=-D
LDFLAG_OUTPUT_linux=-o

CFLAGS_msvc=/c /EHsc
CFLAG_OUTPUT_msvc=/Fo:
CFLAG_INCLUDE_msvc=/I
CFLAG_MACRO_msvc=/D
LDFLAG_OUTPUT_msvc=/Fe:


# old fashioned unix vars:
#LIBS
#LIBPATHS
# newschool:
#DYNAMIC_LIBS
#STATIC_LIBS	<- haven't got this just yet
LIBPATHS_osx+=$(HOME)/lib

DISTDIR_BASE=dist
DISTDIR=$(DISTDIR_BASE)/$(PLATFORM)/$(BUILD)

DIST_PREFIX_osx_lib=lib
DIST_SUFFIX_osx_lib=.dylib
DIST_SUFFIX_osx_app=.app/Contents/MacOS/$(DIST_FILENAME)

DIST_PREFIX_linux_lib=lib
DIST_SUFFIX_linux_lib=.so
DIST_SUFFIX_linux_app=

LIB_SUFFIX=$(DIST_SUFFIX_$(PLATFORM)_lib)

DIST=$(DISTDIR)/$(call concat,$(call buildVar,DIST_PREFIX)$(DIST_FILENAME)$(call buildVar,DIST_SUFFIX))

LDFLAGS_osx_lib+=-dynamiclib -undefined suppress -flat_namespace -install_name @rpath/$(call concat,$(call buildVar,DIST_PREFIX)$(DIST_FILENAME)$(call buildVar,DIST_SUFFIX))
LDFLAGS_linux_lib+=-shared

LDFLAGS_osx_app+=-Wl,-headerpad_max_install_names

.PHONY: default
default: all

.PHONY: all
all:
ifndef PLATFORM
	$(MAKE) help
else
	$(MAKE) build_all
endif

.PHONY: help
help:
	@echo
	@echo "edit Config.mk to specify your configuration"
	@echo " or"
	@echo "make <platform>"
	@echo " or"
	@echo "make <platform>_<build>"
	@echo
	@echo "platform: $(PLATFORM)"
	@echo "build: $(BUILD)"
	@echo

# build config rules
# TODO autogen them based on a list of valid configurations

.PHONY: debug
debug:
ifndef PLATFORM
	$(MAKE) help
else
	$(MAKE) $(PLATFORM)_debug
endif

.PHONY: release
release:
ifndef PLATFORM
	$(MAKE) help
else
	$(MAKE) $(PLATFORM)_release
endif

# platform build rules are repeated for each platform.
# TODO autogen them based on valid platform list?

.PHONY: osx
osx:
	$(MAKE) PLATFORM=osx build_all

.PHONY: linux
linux:
	$(MAKE) PLATFORM=linux build_all

# rest of the build process

.PHONY: build_all
build_all: $(PLATFORM)_debug $(PLATFORM)_release

.PHONY: $(PLATFORM)_debug
$(PLATFORM)_debug:
	$(MAKE) BUILD=debug post_builddist_$(PLATFORM)

.PHONY: $(PLATFORM)_release
$(PLATFORM)_release:
	$(MAKE) BUILD=release post_builddist_$(PLATFORM)

.PHONY: post_builddist_$(PLATFORM)
post_builddist_$(PLATFORM): post_builddist_$(PLATFORM)_$(DIST_TYPE)

.PHONY: post_builddist_$(PLATFORM)_$(DIST_TYPE)
post_builddist_$(PLATFORM)_$(DIST_TYPE):: builddist

post_builddist_osx_app::
	@printf APPLhect > $(dir $(DIST))../PkgInfo
	@echo '<?xml version="1.0" encoding="UTF-8"?>' > $(dir $(DIST))../Info.plist
	@echo '<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> $(dir $(DIST))../Info.plist
	@echo '<plist version="1.0">' >> $(dir $(DIST))../Info.plist
	@echo '<dict>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>CFBundleName</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>$(DIST_FILENAME)</string>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>CFBundleIdentifier</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>net.christopheremoore.$(DIST_FILENAME)</string>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>CFBundleVersion</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>1.0</string>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>CFBundleIconFile</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>Icons</string>' >> $(dir $(DIST))../Info.plist
	
	@echo '	<key>CFBundleDevelopmentRegion</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>English</string>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>CFBundleDocumentTypes</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<array/>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>CFBundleExecutable</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>$(DIST_FILENAME)</string>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>CFBundleInfoDictionaryVersion</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>1.0</string>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>CFBundlePackageType</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>APPL</string>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>CFBundleSignature</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>hect</string>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>NSMainNibFile</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>MainMenu</string>' >> $(dir $(DIST))../Info.plist
	@echo '	<key>NSPrincipalClass</key>' >> $(dir $(DIST))../Info.plist
	@echo '	<string>NSApplication</string>' >> $(dir $(DIST))../Info.plist
	
	@echo '</dict>' >> $(dir $(DIST))../Info.plist
	@echo '</plist>' >> $(dir $(DIST))../Info.plist
	
	-mkdir -p $(dir $(DIST))../Resources/lib
	-cp -R res/* $(DISTDIR)/$(call concat,$(call buildVar,DIST_PREFIX)$(DIST_FILENAME)).app/Contents/Resources/
	@for file in $(DYNAMIC_LIBS) $(call buildVar,DYNAMIC_LIBS); \
	do \
		cp $$file $(dir $(DIST))../Resources/lib; \
		echo install_name_tool -change $$file \@executable_path/../Resources/lib/`basename $$file` $(DIST); \
		install_name_tool -change $$file \@executable_path/../Resources/lib/`basename $$file` $(DIST); \
		echo install_name_tool -change \@rpath/`basename $$file` \@executable_path/../Resources/lib/`basename $$file` $(DIST); \
		install_name_tool -change \@rpath/`basename $$file` \@executable_path/../Resources/lib/`basename $$file` $(DIST); \
	done;
	@echo done with $(DIST)
	@echo

.PHONY: builddist
builddist: CFLAGS+= $(call buildVar,CFLAGS)
builddist: CFLAGS+= $(addprefix -I,$(INCLUDE) $(call buildVar,INCLUDE))
builddist: CFLAGS+= $(addprefix -D,$(MACROS) $(call buildVar,MACROS))
builddist: LDFLAGS+= $(call buildVar,LDFLAGS)
builddist: DEPLIBS+= $(addprefix -l,$(LIBS) $(call buildVar,LIBS))
builddist: DEPLIBS+= $(addprefix -L,$(LIBPATHS) $(call buildVar,LIBPATHS))
builddist: DEPLIBS+= $(DYNAMIC_LIBS) $(call buildVar,DYNAMIC_LIBS)
builddist: $(DIST)

#builddist: LDFLAGS+= $(realpath $(DYNAMIC_LIBS) $(call buildVar,DYNAMIC_LIBS))

$(OBJDIR)/%.o : $(SRCDIR_BASE)/%.cpp $(foreach file,$(INCLUDE), $(shell $(FIND) $(file) -type f))
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) $(call buildVar,CFLAG_OUTPUT) $@ $<

# .o files must go before their dependencies
# but I'm adding those dependencies inside the LDFLAGS
# so this means .o files will go before their flag definitions
# (unless I separate the lib dependencies out of LDFLAGS ...)
$(DIST):: $(OBJPATHS)
	-mkdir -p $(@D)
	$(LD) $(LDFLAGS) $^ $(DEPLIBS) $(call buildVar,LDFLAG_OUTPUT) $@

.PHONY: clean
clean:
	-rm -fr $(OBJDIR_BASE)

.PHONY: distclean
distclean:
	-rm -fr $(DISTDIR_BASE)

