TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS_RELEASE = -O3
QMAKE_CXXFLAGS_DEBUG = -g
QMAKE_CXXFLAGS += -std=c++11


#default extern libraries
DEFINES+= \
  BUILD_ON_ANIMAL # Set some paths related to the animal build-server.

HEADERS += \
    ../inc/pag_muxilp.h \
    ../inc/helper.h \
    ../inc/triplet.h #RPAG triplet


SOURCES += \
    ../src/main.cpp \
    ../src/pag_muxilp.cpp

INCLUDEPATH += \
    ../inc

## build on animal-server
# Set the paths to build on the digi-animal-server.
contains(DEFINES,BUILD_ON_ANIMAL){
  DEFINES+= USE_OLD_LIBSTDCXX
}

#use GurobiILP-Wrapper
unix: LIBS += -L$$PWD/../../../../scalp/trunk/lib/ -lILP

INCLUDEPATH += $$PWD/../../../../scalp/trunk/include
DEPENDPATH += $$PWD/../../../../scalp/trunk/include

unix: LIBS += -L$$PWD/../../scalp/trunk/lib/ -lILP-Gurobi

INCLUDEPATH += $$PWD/../../../../scalp/trunk/include
DEPENDPATH += $$PWD/../../../../scalp/trunk/include

unix: LIBS += -L/opt/gurobi/gurobi600/linux64/lib/ -lgurobi60

INCLUDEPATH += /opt/gurobi/gurobi600/linux64/include
DEPENDPATH +=/opt/gurobi/gurobi600/linux64/include

unix:!macx: LIBS += -L$$PWD/../../../../pagsuite/trunk/lib/ -lpag_lib

INCLUDEPATH += $$PWD/../../../../pagsuite/trunk/pag_lib/paglib_copa/inc
DEPENDPATH += $$PWD/../../../../pagsuite/trunk/pag_lib/paglib_copa/inc
