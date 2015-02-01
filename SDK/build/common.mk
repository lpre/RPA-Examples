
OBJS=$(SOURCES:.cpp=.o)

LIBOBJS=$(LIBSOURCES:.cpp=.o)


ifeq ($(PLATFORM),win32)
include win32.mk
endif

ifeq ($(PLATFORM),win64)
include win64.mk
endif

ifeq ($(PLATFORM),linux32)
include linux32.mk
endif

ifeq ($(PLATFORM),linux64)
include linux64.mk
endif

ifeq ($(PLATFORM),macosx)
include macosx.mk
endif
