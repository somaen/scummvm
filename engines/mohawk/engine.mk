ifdef ENABLE_MOHAWK
DEFINES += -DENABLE_MOHAWK=$(ENABLE_MOHAWK)
MODULES += engines/mohawk

ifdef ENABLE_CSTIME
DEFINES += -DENABLE_CSTIME
endif

ifdef ENABLE_MYST
DEFINES += -DENABLE_MYST
endif

ifdef ENABLE_RIVEN
DEFINES += -DENABLE_RIVEN
endif
endif
