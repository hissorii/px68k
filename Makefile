include version.txt

CC	 = gcc
CXX	 = c++
CXXLINK	 = $(CXX)
RM	 = rm -f
TAGS	 = etags
DEPEND	 = gccmakedep
DEPEND_DEFINES =

# for debug
CDEBUGFLAGS = -g -O0 -fno-strict-aliasing

#
# enable SDL_gfx
#
CDEBUGFLAGS+= -DUSE_SDLGFX

#
# disable sound
#
#CDEBUGFLAGS+= -DNO_SOUND

#
# disable mercury unit
#
CDEBUGFLAGS+= -DNO_MERCURY

#
# enable RFMDRV
#
#CDEBUGFLAGS+= -DRFMDRV

#
# for Opt.
#
# CDEBUGFLAGS= -O3
# CDEBUGFLAGS+= -funroll-loops
# CDEBUGFLAGS+= -fomit-frame-pointer
# CDEBUGFLAGS+= -ffast-math

# CDEBUGFLAGS+= -march=pentium-m
# CDEBUGFLAGS+= -msse -mfpmath=sse

#
# for DEBUG
#
# CDEBUGFLAGS= -O0
# CDEBUGFLAGS+= -g
# CDEBUGFLAGS+= -W -Wall -Wuninitialized
# CDEBUGFLAGS+= -Wunused
# CDEBUGFLAGS+= -Werror
# CDEBUGFLAGS+= -DINLINE=
# CDEBUGFLAGS+= -DUSE_GAS

CDEBUGFLAGS+=-DPX68K_VERSION=$(PX68K_VERSION)

ifdef SDL2
# SDL 2.0
SDL_CONFIG?= sdl2-config
else
# SDL 1.2
SDL_CONFIG?= sdl-config
endif

SDL_INCLUDE=	`$(SDL_CONFIG) --cflags`
ifdef SDL2
SDL_LIB=	`$(SDL_CONFIG) --libs` -lSDL2_gfx
else
SDL_LIB=	`$(SDL_CONFIG) --libs` -lSDL_gfx
endif

ifeq ($(shell uname -m),armv6l)
	MOPT=
else
	MOPT= -m32
endif

LDLIBS = -lm

EXTRA_INCLUDES= -I./x11 -I./x68k -I./fmgen -I./win32api $(SDL_INCLUDE)

CXXDEBUGFLAGS= $(CDEBUGFLAGS)

CFLAGS= $(MOPT) $(CDEBUGFLAGS) $(EXTRA_INCLUDES)
CXXFLAGS= $(MOPT) $(CXXDEBUGFLAGS) $(EXTRA_INCLUDES)
CXXLDOPTIONS= $(CXXDEBUGFLAGS)

CPUOBJS= x68k/d68k.o m68000/c68k.o m68000/m68000.o

X68KOBJS= x68k/adpcm.o x68k/bg.o x68k/crtc.o x68k/dmac.o x68k/fdc.o x68k/fdd.o x68k/disk_d88.o x68k/disk_dim.o x68k/disk_xdf.o x68k/gvram.o x68k/ioc.o x68k/irqh.o x68k/mem_wrap.o x68k/mercury.o x68k/mfp.o x68k/palette.o x68k/midi.o x68k/pia.o x68k/rtc.o x68k/sasi.o x68k/scc.o x68k/scsi.o x68k/sram.o x68k/sysport.o x68k/tvram.o

FMGENOBJS= fmgen/fmgen.o fmgen/fmg_wrap.o fmgen/file.o fmgen/fmtimer.o fmgen/opm.o fmgen/opna.o fmgen/psg.o

X11OBJS= x11/joystick.o x11/juliet.o x11/keyboard.o x11/mouse.o x11/prop.o x11/status.o x11/timer.o x11/dswin.o x11/windraw.o x11/winui.o x11/about.o x11/common.o

X11CXXOBJS= x11/winx68k.o

WIN32APIOBJS= win32api/dosio.o win32api/fake.o win32api/peace.o

COBJS=		$(X68KOBJS) $(X11OBJS) $(WIN32APIOBJS) $(CPUOBJS)
CXXOBJS=	$(FMGENOBJS) $(X11CXXOBJS)
OBJS=		$(COBJS) $(CXXOBJS)

CSRCS=		$(COBJS:.o=.c)
CXXSRCS=	$(CXXOBJS:.o=.cpp)
SRCS=		$(CSRCS) $(CXXSRCS)

.SUFFIXES: .c .cpp

.c.o:
	$(CC) -o $@ $(CFLAGS) -c $*.c

.cpp.o:
	$(CXX) -o $@ $(CXXFLAGS) -c $*.cpp

all:: px68k

px68k: $(OBJS)
	$(RM) $@
	$(CXXLINK) $(MOPT) -o $@ $(CXXLDOPTIONS) $(OBJS) $(SDL_LIB) $(LDLIBS)

depend::
	$(DEPEND) -- $(CXXFLAGS) $(DEPEND_DEFINES) -- $(SRCS)

clean::
	$(RM) px68k
	$(RM) $(OBJS)
	$(RM) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut   "#"*

tags::
	find . -name "*.h" -o -name "*.c" -o -name "*.cpp" | $(TAGS) -

