ifdef ENABLE_SAGA
DEFINES += -DENABLE_SAGA=$(ENABLE_SAGA)
MODULES += engines/saga

ifdef ENABLE_IHNM
DEFINES += -DENABLE_IHNM
endif

ifdef ENABLE_SAGA2
DEFINES += -DENABLE_SAGA2
endif
endif
