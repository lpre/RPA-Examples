RM = del /Q
CP = xcopy /Y
XCP = xcopy /Y /S /E
MV = move /Y

CC = cl
MT = mt

INCLUDES = \
	/I "..\..\third_parties\libconfig-1.4.5\lib" \
	/I "..\..\libutils\include" \
	/I "..\..\libmath\include" \
	/I "..\..\librpa\include" \
	/I "..\..\libnozzle\include" \
	/I "..\..\libthermo\include" \
	/I "..\..\libdesign\include" \
	/I "..\..\libflow\include"

WLIBS = \
	/link libconfig++.lib libutils.lib libmath.lib librpa.lib libnozzle.lib libthermo.lib libdesign.lib  libflow.lib wrapper.lib \
	/LIBPATH:"..\..\third_parties\bin\$(ARCH)" \
	/LIBPATH:"..\..\libutils\bin\$(ARCH)" \
	/LIBPATH:"..\..\libmath\bin\$(ARCH)" \
	/LIBPATH:"..\..\librpa\bin\$(ARCH)" \
	/LIBPATH:"..\..\libnozzle\bin\$(ARCH)" \
	/LIBPATH:"..\..\libthermo\bin\$(ARCH)" \
	/LIBPATH:"..\..\libdesign\bin\$(ARCH)" \
	/LIBPATH:"..\..\libflow\bin\$(ARCH)"

LIBS = \
	/link libconfig++.lib libutils.lib libmath.lib librpa.lib libnozzle.lib libthermo.lib libdesign.lib libflow.lib \
	/LIBPATH:"..\..\third_parties\bin\$(ARCH)" \
	/LIBPATH:"..\..\libutils\bin\$(ARCH)" \
	/LIBPATH:"..\..\libmath\bin\$(ARCH)" \
	/LIBPATH:"..\..\librpa\bin\$(ARCH)" \
	/LIBPATH:"..\..\libnozzle\bin\$(ARCH)" \
	/LIBPATH:"..\..\libthermo\bin\$(ARCH)" \
	/LIBPATH:"..\..\libdesign\bin\$(ARCH)" \
	/LIBPATH:"..\..\libflow\bin\$(ARCH)"


CFLAGS_OPT  = /MP /O2 /EHsc /w $(INCLUDES) /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" 

ifneq ($(ARCH),win64)
CFLAGS_OPT  += /arch:SSE2
endif

CFLAGS_LIB  = $(CFLAGS_OPT) /MD /c

CFLAGS  = $(CFLAGS_OPT) /D "SHARED" /MD /D "_BIND_TO_CURRENT_CRT_VERSION=1"

SHARED_CFLAGS  = $(CFLAGS) /D "_USRDLL" /LD


all: init dll clean exe clean_lib

init:
		if not exist ..\bin mkdir ..\bin
		$(RM) ..\bin\* 

dll:
ifdef LIBSOURCES
		$(CC) $(SHARED_CFLAGS) /Fe$(LIBNAME).dll $(LIBSOURCES) $(LIBS)
		$(MT) -nologo -manifest $(LIBNAME).dll.manifest.xml -outputresource:$(LIBNAME).dll;2
		$(MV) $(LIBNAME).dll "..\bin\"
endif

exe:
ifndef LIBSOURCES
		$(CC) $(CFLAGS) /Fe..\bin\$(EXENAME).exe $(SOURCES) $(LIBS)
else
		$(CC) $(CFLAGS) /Fe..\bin\$(EXENAME).exe $(SOURCES) $(WLIBS)
		$(CP) ..\src\*.py "..\bin\"
endif		
		$(XCP) ..\..\files "..\bin\"
		$(CP) ..\..\third_parties\bin\$(ARCH)\*.dll "..\bin\"
		$(CP) ..\..\libutils\bin\$(ARCH)\*.dll "..\bin\"
		$(CP) ..\..\libmath\bin\$(ARCH)\*.dll "..\bin\"
		$(CP) ..\..\librpa\bin\$(ARCH)\*.dll "..\bin\"
		$(CP) ..\..\libnozzle\bin\$(ARCH)\*.dll "..\bin\"
		$(CP) ..\..\libthermo\bin\$(ARCH)\*.dll "..\bin\"
		$(CP) ..\..\libdesign\bin\$(ARCH)\*.dll "..\bin\"
		$(CP) ..\..\libflow\bin\$(ARCH)\*.dll "..\bin\"

clean:
		$(RM) *.obj

clean_lib:
		$(RM) *.obj
ifdef LIBSOURCES
		$(RM) *.lib
		$(RM) *.exp
endif



