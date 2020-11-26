#
#   Copyright (C) MINZKN.COM
#   All rights reserved.
#
#   Maintainers
#     JaeHyuk Cho <mailto:minzkn@minzkn.com>
#

# check for minimal make version (NOTE: this check will break at make 10.x !)
override DEF_HWPORT_REQUIRE_MINIMUM_MAKE_VERSION:=3.81#
ifneq ($(firstword $(sort $(MAKE_VERSION) $(DEF_HWPORT_REQUIRE_MINIMUM_MAKE_VERSION))),$(DEF_HWPORT_REQUIRE_MINIMUM_MAKE_VERSION))
$(error you have make "$(MAKE_VERSION)". GNU make >= $(DEF_HWPORT_REQUIRE_MINIMUM_MAKE_VERSION) is required !)
endif

SHELL_BASH :=$(wildcard /bin/bash)#
ifneq ($(SHELL_BASH),)
SHELL :=$(SHELL_BASH)# bash shell default using
else
SHELL ?=/bin/sh#
endif
MAKE ?=make#

# Delete default rules. We don't use them. This saves a bit of time.
.SUFFIXES:

# .EXPORT_ALL_VARIABLES: # DO NOT USE !
MAKEFLAGS                    ?=#
export MAKEFLAGS
export PATH

HOST_NAME :=$(shell hostname --short)#
HOST_USER :=$(shell whoami)#
HOST_ARCH :=$(shell echo "$(shell uname -m)" | sed \
    -e s/sun4u/sparc64/ \
    -e s/arm.*/arm/ \
    -e s/sa110/arm/ \
    -e s/s390x/s390/ \
    -e s/parisc64/parisc/ \
    -e s/ppc.*/powerpc/ \
    -e s/mips.*/mips/ \
)# auto detect architecture
HOST_OS :=$(shell echo "$(shell uname)" | sed \
    -e  s/Linux/linux/ \
    -e  s/Darwin/darwin/ \
)# auto detect os
HOST_VENDOR :=pc#
HOST_LIBC :=gnu#
HOST_LABEL :=$(HOST_ARCH)#
HOST_BUILD_PROFILE :=$(HOST_ARCH)-$(HOST_VENDOR)-$(HOST_OS)-$(HOST_LIBC)#

TARGET_ARCH :=$(HOST_ARCH)#
TARGET_VENDOR :=$(HOST_VENDOR)#
TARGET_OS :=$(HOST_OS)#
TARGET_LIBC :=$(HOST_LIBC)#
TARGET_LABEL :=$(TARGET_ARCH)#
TARGET_BUILD_PROFILE :=$(TARGET_ARCH)-$(TARGET_VENDOR)-$(TARGET_OS)-$(TARGET_LIBC)#

EXT_DEPEND :=.d#
EXT_C_SOURCE :=.c#
EXT_CXX_SOURCE :=.cpp#
EXT_C_HEADER :=.h#
EXT_CXX_HEADER :=.h#
EXT_OBJECT :=.o#
EXT_LINK_OBJECT :=.lo#
EXT_ARCHIVE :=.a#
EXT_SHARED :=.so#
EXT_EXEC :=#
EXT_CONFIG :=.conf#

KERNEL_DIR ?=#

CROSS_COMPILE :=#

ECHO :=echo#
MKDIR :=mkdir -p#
TOUCH :=touch#
SYMLINK :=ln -sf#
SED :=sed#
INSTALL :=install#
INSTALL_BIN :=$(INSTALL) -m0755#
INSTALL_LIB :=$(INSTALL) -m0755#
INSTALL_DIR :=$(INSTALL) -d -m0755#
INSTALL_DATA :=$(INSTALL) -m0644#
INSTALL_CONF :=$(INSTALL) -m0644#

CC := $(CROSS_COMPILE)gcc#
LD := $(CROSS_COMPILE)ld#
AR := $(CROSS_COMPILE)ar#
RM := rm -f#
RMDIR := rm -rf#
COPY_FILE := cp -f#
CAT :=cat#
STRIP := $(CROSS_COMPILE)strip#

THIS_NAME :=netlink_iotop#
THIS_LIBNAME :=$(THIS_NAME)#
THIS_VERSION :=0.0.1# library version
THIS_INTERFACE_VERSION :=0# library interface version

PATH_CURRENT :=$(abspath .)#
PATH_SOURCE :=$(PATH_CURRENT)/source#
PATH_INCLUDE :=$(PATH_CURRENT)/include#
PATH_OBJS :=$(PATH_CURRENT)/objs#
PATH_STAGE1 :=$(PATH_OBJS)/$(TARGET_BUILD_PROFILE)#
DESTDIR :=$(PATH_CURRENT)/rootfs# default staging(stage2) directory

CPPFLAGS_COMMON :=#
CPPFLAGS :=#
CFLAGS_COMMON :=#
CFLAGS :=#
LDFLAGS_COMMON :=#
LDFLAGS :=#
LDFLAGS_EXEC :=-rdynamic -fPIE -pie#
LDFLAGS_SHARED_COMMON :=#
LDFLAGS_SHARED_LINK :=#
LDFLAGS_SHARED :=#
ARFLAGS_COMMON :=#
ARFLAGS :=#
LIBS_COMMON :=#
LIBS :=#

CFLAGS_COMMON +=-O2#
#CFLAGS_COMMON +=-g#
CFLAGS_COMMON +=-pipe#
CFLAGS_COMMON +=-fPIC#
#CFLAGS_COMMON +=-fomit-frame-pointer# backtrace() daes not work !
CFLAGS_COMMON +=-fno-omit-frame-pointer# backtrace() will work normally.
CFLAGS_COMMON +=-ansi#
CFLAGS_COMMON +=-Wall -W#
CFLAGS_COMMON +=-Wshadow#
CFLAGS_COMMON +=-Wcast-qual#
CFLAGS_COMMON +=-Wcast-align#
CFLAGS_COMMON +=-Wpointer-arith#
CFLAGS_COMMON +=-Wbad-function-cast#
CFLAGS_COMMON +=-Wstrict-prototypes#
CFLAGS_COMMON +=-Wmissing-prototypes#
CFLAGS_COMMON +=-Wmissing-declarations#
CFLAGS_COMMON +=-Wnested-externs#
CFLAGS_COMMON +=-Winline#
CFLAGS_COMMON +=-Wwrite-strings#
CFLAGS_COMMON +=-Wchar-subscripts#
CFLAGS_COMMON +=-Wformat#
CFLAGS_COMMON +=-Wformat-security#
CFLAGS_COMMON +=-Wimplicit#
CFLAGS_COMMON +=-Wmain#
CFLAGS_COMMON +=-Wmissing-braces#
CFLAGS_COMMON +=-Wnested-externs#
CFLAGS_COMMON +=-Wparentheses#
CFLAGS_COMMON +=-Wredundant-decls#
CFLAGS_COMMON +=-Wreturn-type#
CFLAGS_COMMON +=-Wsequence-point#
CFLAGS_COMMON +=-Wsign-compare#
CFLAGS_COMMON +=-Wswitch#
CFLAGS_COMMON +=-Wuninitialized#
CFLAGS_COMMON +=-Wunknown-pragmas#
CFLAGS_COMMON +=-Wcomment#
CFLAGS_COMMON +=-Wundef#
CFLAGS_COMMON +=-Wunused#
#CFLAGS_COMMON +=-Wunreachable-code#
#CFLAGS_COMMON +=-Wconversion#
#CFLAGS_COMMON +=-Wpadded#
CFLAGS_COMMON +=-I.#
ifneq ($(wildcard $(PATH_SOURCE)),)
CFLAGS_COMMON +=-I$(PATH_SOURCE)#
endif
ifneq ($(wildcard $(PATH_INCLUDE)),)
CFLAGS_COMMON +=-I$(PATH_INCLUDE)#
endif
ifneq ($(KERNEL_DIR),)
CFLAGS_COMMON +=-I$(KERNEL_DIR)/include#
endif

CPPFLAGS_COMMON +=-D_REENTRANT# thread safety (optional)
CPPFLAGS_COMMON +=-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64# enable 64-bits file i/o compatibility (optional)
CPPFLAGS_COMMON +=-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0# glibc run-time compatibility compile (optional)

ifneq ($(wildcard version.h),)
CPPFLAGS_COMMON +=-DHAVE_VERSION_H=1#
endif

LDFLAGS_SHARED_COMMON +=-L.#
#LDFLAGS_SHARED_LINK +=-l<mylib-name>#
LDFLAGS +=-s#
#LIBS_COMMON +=-ldl#
#LIBS_COMMON +=-lpthread#

ARFLAGS_COMMON +=rcs#

TARGET :=$(PATH_STAGE1)/$(THIS_NAME)$(EXT_EXEC)# executable shared object
#TARGET +=$(PATH_STAGE1)/lib$(THIS_LIBNAME)$(EXT_LINK_OBJECT)# object library
#TARGET +=$(PATH_STAGE1)/lib$(THIS_LIBNAME)$(EXT_ARCHIVE)# static library
#TARGET +=$(PATH_STAGE1)/lib$(THIS_LIBNAME)$(EXT_SHARED)# shared library

# default make goal
.PHONY: all world rebuild install
all world: __build_all
rebuild: clean all

install: all
	@$(ECHO) "[**] installing (DESTDIR=\"$(DESTDIR)\")"
	@$(INSTALL_DIR) "$(DESTDIR)/"
	@$(INSTALL_DIR) "$(DESTDIR)/usr/"
	@$(INSTALL_DIR) "$(DESTDIR)/usr/bin/"
	@$(INSTALL_BIN) "$(PATH_STAGE1)/$(THIS_NAME)$(EXT_EXEC)" "$(DESTDIR)/usr/bin/"
	@$(ECHO) "[**] installed (DESTDIR=\"$(DESTDIR)\")"

# clean project
.PHONY: distclean clean
distclean clean:
	@$(ECHO) "[**] $(@)"
ifneq ($(wildcard $(PATH_OBJS)),)
	@$(RMDIR) $(wildcard $(PATH_OBJS))
endif

.PHONY: check
check:
	@$(ECHO) "THIS_NAME: $(THIS_NAME)"

# real build depend
.PHONY: __build_all
__build_all: $(PATH_STAGE1)/.keep $(TARGET)
	@$(ECHO) "[**] build complete ($(^))"
	$(if $(THIS_VERSION),@$(ECHO) "   - THIS_VERSION=\"$(THIS_VERSION)\"")
	$(if $(TARGET_BUILD_PROFILE),@$(ECHO) "   - TARGET_BUILD_PROFILE=\"$(TARGET_BUILD_PROFILE)\"")
	$(if $(CROSS_COMPILE),@$(ECHO) "   - CROSS_COMPILE=\"$(CROSS_COMPILE)\"")
	$(if $(strip $(CPPFLAGS_COMMON) $(CPPFLAGS)),@$(ECHO) "   - CPPFLAGS=\"$(strip $(CPPFLAGS_COMMON) $(CPPFLAGS))\"")
	$(if $(strip $(CFLAGS_COMMON) $(CFLAGS)),@$(ECHO) "   - CFLAGS=\"$(strip $(CFLAGS_COMMON) $(CFLAGS))\"")
	$(if $(strip $(LDFLAGS_COMMON) $(LDFLAGS)),@$(ECHO) "   - LDFLAGS=\"$(strip $(LDFLAGS_COMMON) $(LDFLAGS))\"")
	$(if $(strip $(LDFLAGS_EXEC)),@$(ECHO) "   - LDFLAGS_EXEC=\"$(strip $(LDFLAGS_EXEC))\"")
	$(if $(strip $(LDFLAGS_SHARED_COMMON) $(LDFLAGS_SHARED)),@$(ECHO) "   - LDFLAGS_SHARED=\"$(strip $(LDFLAGS_SHARED_COMMON) $(LDFLAGS_SHARED))\"")
	$(if $(strip $(ARFLAGS_COMMON) $(ARFLAGS)),@$(ECHO) "   - LDFLAGS_SHARED=\"$(strip $(ARFLAGS_COMMON) $(ARFLAGS))\"")
	$(if $(strip $(LIBS_COMMON) $(LIBS)),@$(ECHO) "   - LIBS=\"$(strip $(LIBS_COMMON) $(LIBS))\"")
	$(if $(TARGET),@$(ECHO) "   - TARGET=\"$(TARGET)\"")

$(PATH_STAGE1)/.keep:
	@$(ECHO) "[**] create stage1 directory ($(dir $(@)))"
	@$(MKDIR) "$(dir $(@))"

# exec link (-fPIE -pie => shared object build)
MAIN_SOURCE_LIST :=main$(EXT_C_SOURCE)#
#MAIN_SOURCE_LIST :=$(notdir $(wildcard $(PATH_SOURCE)/*$(EXT_C_SOURCE)))# auto detect source
ifneq ($(strip $(MAIN_SOURCE_LIST)),)
MAIN_OBJECTS :=$(MAIN_SOURCE_LIST:%$(EXT_C_SOURCE)=$(PATH_STAGE1)/%$(EXT_OBJECT))# auto generate object by source
#$(PATH_STAGE1)/$(THIS_NAME)$(EXT_EXEC): LDFLAGS_SHARED_LINK+=
$(PATH_STAGE1)/$(THIS_NAME)$(EXT_EXEC): $(MAIN_OBJECTS)
	@$(ECHO) "[LD] $(notdir $(@)) <= $(notdir $(^)) (LDFLAGS=\"$(strip $(LDFLAGS_SHARED_COMMON) $(LDFLAGS_SHARED) $(LDFLAGS_COMMON) $(LDFLAGS) $(LDFLAGS_EXEC))\", LIBS=\"$(strip $(LIBS_COMMON) $(LIBS))\")"
	@$(CC) $(LDFLAGS_SHARED_COMMON) $(LDFLAGS_SHARED) $(LDFLAGS_COMMON) $(LDFLAGS) $(LDFLAGS_EXEC) -o "$(@)" $(^) $(LDFLAGS_SHARED_LINK) $(LIBS_COMMON) $(LIBS)
	@$(STRIP) --remove-section=.comment --remove-section=.note $(@) # strong strip (optional)
$(MAIN_OBJECTS): CFLAGS_COMMON+=-fPIE
endif

# library link
LIBRARY_SOURCE_LIST :=$(filter-out $(MAIN_SOURCE_LIST),$(notdir $(wildcard $(PATH_SOURCE)/*$(EXT_C_SOURCE))))# auto detect source
ifneq ($(strip $(LIBRARY_SOURCE_LIST)),)
LIBRARY_OBJECTS :=$(LIBRARY_SOURCE_LIST:%$(EXT_C_SOURCE)=$(PATH_STAGE1)/%$(EXT_OBJECT))# auto generate object by source
$(PATH_STAGE1)/lib$(THIS_LIBNAME)$(EXT_LINK_OBJECT): $(LIBRARY_OBJECTS)
	@$(ECHO) "[LO] $(notdir $(@)) <= $(notdir $(^)) (LDFLAGS=\"$(strip $(LDFLAGS_COMMON) $(LDFLAGS))\")"
	@$(LD) $(LDFLAGS_COMMON) $(LDFLAGS) -r -o "$(@)" $(^)
$(PATH_STAGE1)/lib$(THIS_LIBNAME)$(EXT_ARCHIVE): $(LIBRARY_OBJECTS)
	@$(ECHO) "[AR] $(notdir $(@)) <= $(notdir $(^)) (ARFLAGS=\"$(strip $(ARFLAGS_COMMON) $(ARFLAGS))\")"
	@$(foreach s_this_name,$(?),$(AR) $(ARFLAGS_COMMON) $(ARFLAGS) "$(@)" "$(s_this_name)" 2>/dev/null;)
$(PATH_STAGE1)/lib$(THIS_LIBNAME)$(EXT_SHARED): $(LIBRARY_OBJECTS)
	@$(ECHO) "[SO] $(notdir $(@)).$(THIS_VERSION) <= $(notdir $(^)) (soname=$(@).$(THIS_INTERFACE_VERSION), LDFLAGS=\"$(strip $(LDFLAGS_SHARED_COMMON) $(LDFLAGS_SHARED) $(LDFLAGS_COMMON) $(LDFLAGS) $(LDFLAGS_SHARED_LINK))\" LIBS=\"$(strip $(LIBS_COMMON) $(LIBS))\")"
	@$(CC) $(LDFLAGS_SHARED_COMMON) $(LDFLAGS_SHARED) $(LDFLAGS_COMMON) $(LDFLAGS) -shared -Wl,-soname,$(@).$(THIS_INTERFACE_VERSION) -o "$(@).$(THIS_VERSION)" $(^) $(LDFLAGS_SHARED_LINK) $(LIBS_COMMON) $(LIBS)
	@$(ECHO) "[SL] $(notdir $(@)).$(THIS_INTERFACE_VERSION) <= $(notdir $(@)).$(THIS_VERSION)"
	@$(SYMLINK) "$(@).$(THIS_VERSION)" "$(@).$(THIS_INTERFACE_VERSION)"
	@$(ECHO) "[SL] $(notdir $(@)) <= $(notdir $(@)).$(THIS_VERSION)"
	@$(SYMLINK) "$(@).$(THIS_VERSION)" "$(@)"
endif

# common compile
$(PATH_STAGE1)/%$(EXT_OBJECT): $(PATH_SOURCE)/%$(EXT_C_SOURCE) Makefile
	@$(ECHO) "[CC] $(notdir $(@)) <= $(notdir $(<))"
	@$(CC) $(CFLAGS_COMMON) $(CFLAGS) $(CPPFLAGS_COMMON) $(CPPFLAGS) -c -o "$(@)" "$(<)"
	@$(CC) -MMD $(CFLAGS_COMMON) $(CFLAGS) $(CPPFLAGS_COMMON) $(CPPFLAGS) -c -o "$(@)" "$(<)" # create depend rule file (strong depend check, optional)

# include depend rules (strong depend check, optional)
override THIS_DEPEND_RULES_LIST:=$(wildcard *$(EXT_DEPEND))#
ifneq ($(THIS_DEPEND_RULES_LIST),)
sinclude $(THIS_DEPEND_RULES_LIST)
endif

.DEFAULT:
	@$(ECHO) "[!!] unknown goal ($(@))"

# End of makefile.gnu
