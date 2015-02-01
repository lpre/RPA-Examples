RM = rm -rf
CP = cp -f -d
MV = mv -f

CC = g++
AR = ar

INCLUDES = \
	-I../../third_parties/libconfig-1.4.5/lib \
	-I../../libutils/include \
	-I../../libmath/include \
	-I../../librpa/include \
	-I../../libnozzle/include \
	-I../../libthermo/include \
	-I../../libdesign/include \
	-I../../libflow/include

LIBS = \
	-L../../third_parties/bin/$(ARCH) \
	-L../../libutils/bin/$(ARCH) \
	-L../../libmath/bin/$(ARCH) \
	-L../../librpa/bin/$(ARCH) \
	-L../../libnozzle/bin/$(ARCH) \
	-L../../libthermo/bin/$(ARCH) \
	-L../../libdesign/bin/$(ARCH) \
	-L../../libflow/bin/$(ARCH) \
	-L$(SLIB) \
	-lconfig++ -lutils -lmath -lrpa -lnozzle -lthermo -ldesign -lflow -lm

CFLAGS  = $(FLAG) -LNO:vintr=2 -LNO:simd=2 -Wall -fmessage-length=0 -pthread -std=c++0x \
		  -falign-functions -falign-loops \
		  $(INCLUDES) $(LIBS) \
		  -g0 -O3 -mssse3 -mtune=generic -fno-math-errno -finline-functions -c

SHARED_CFLAGS  = $(CFLAGS) -fPIC -DSHARED -D__dll_impl__

LDFLAGS = $(FLAG) -lpthread -s

all: init dll exe clean
		echo Done

init:
		mkdir -p ../bin 
		$(RM) ../bin/*

$(OBJS): $(SOURCES)
		$(CC) $(CFLAGS) $(INCLUDES) $(patsubst %.o,%.cpp,$@)

dll:
ifdef LIBSOURCES
		$(CC) $(SHARED_CFLAGS) /Fe$(LIBNAME).dll $(LIBSOURCES) $(LIBS)
		$(MT) -nologo -manifest $(LIBNAME).dll.manifest.xml -outputresource:$(LIBNAME).dll;2
		$(MV) $(LIBNAME).dll "..\bin\"
endif

exe: $(OBJS)
		$(CC) $(FLAG) $(LIBS) $(notdir $(OBJS)) -o ../bin/$(EXENAME)
		$(RM) *.o
#		strip --strip-unneeded ../bin/$(EXENAME)
		$(CP) -r ../../files/* ../bin/
		$(CP) ../../third_parties/bin/$(ARCH)/*.* ../bin/
		$(CP) ../../libutils/bin/$(ARCH)/*.* ../bin/
		$(CP) ../../libmath/bin/$(ARCH)/*.* ../bin/
		$(CP) ../../librpa/bin/$(ARCH)/*.* ../bin/
		$(CP) ../../libnozzle/bin/$(ARCH)/*.* ../bin/
		$(CP) ../../libthermo/bin/$(ARCH)/*.* ../bin/
		$(CP) ../../libdesign/bin/$(ARCH)/*.* ../bin/
		$(CP) ../../libflow/bin/$(ARCH)/*.* ../bin/
		echo "#!/bin/sh" > ../bin/$(EXENAME).exe
#		echo "LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./$(EXENAME) $@" >> ../bin/$(EXENAME).exe
		echo "LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./$(EXENAME)" >> ../bin/$(EXENAME).exe
		chmod +x ../bin/$(EXENAME).exe

clean:
		$(RM) $(OBJS)
		$(RM) *.o

