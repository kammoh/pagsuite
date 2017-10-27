#  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
#  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
#  You can use and/or modify it for research purposes, for commercial use, please ask for a license.
#
#  For more information please visit http://www.uni-kassel.de/go/pagsuite.

TARGET = tscmeval
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   -= qt
DEFINES  -= UNICODE QT_LARGEFILE_SUPPORT

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE += -O3

TEMPLATE = app

linux-g++{
    message("Build for UNIX.")
    DEFINES += OS_LINUX
}
macx{
    message("Build for MAC.")
    DEFINES += OS_MAC
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11 #must be >= 10.9 (c++v11 support)
}
win32{
    message("Build for WIN.")
    DEFINES += OS_WIN
}

INCLUDEPATH += ../inc
INCLUDEPATH += ../../rpag/inc/

SOURCES += \
    ../src/main.cpp \
    ../../rpag/src/debug.cpp \
    ../../rpag/src/compute_successor_set.cpp \
    ../../rpag/src/rpag_functions.cpp \
    ../../rpag/src/fundamental.cpp \
    ../../rpag/src/log2_64.cpp \
    ../../rpag/src/norm.cpp \
    ../src/compute_realization.cpp \
    ../src/evaluate_ternary_scm_topologies.cpp

HEADERS += \
    ../../rpag/inc/debug.h \
    ../../rpag/inc/compute_successor_set.h \
    ../../rpag/inc/rpag_functions.h \
    ../../rpag/inc/fundamental.h \
    ../../rpag/inc/log2_64.h \
    ../../rpag/inc/norm.h \
    ../inc/compute_realization.h \
    ../src/evaluate_ternary_scm_topologies.h
