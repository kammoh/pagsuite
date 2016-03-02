#  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
#  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
#  You can use and/or modify it for research purposes, for commercial use, please ask for a license.
#
#  For more information please visit http://www.uni-kassel.de/go/pagsuite.

TARGET = opag

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE += -O3


#SOURCES += ../src/main.cpp

SOURCES += \
    ../src/main.cpp \
    ../src/opag.cpp \
    ../../rpag/src/compute_successor_set.cpp \
    ../../rpag/src/norm.cpp \
    ../../rpag/src/log2_64.cpp \
    ../../rpag/src/csd.cpp \
    ../../rpag/src/debug.cpp \
    ../../rpag/src/rpag_functions.cpp \
    ../src/adder_depth.cpp

HEADERS += \
    ../../rpag/inc/compute_successor_set.h \
    ../../rpag/inc/csd.h \
    ../../rpag/inc/log2_64.h \
    ../../rpag/inc/fundamental.h \
    ../../rpag/inc/rpag_functions.h \
    ../inc/opag.h \
    ../inc/adder_depth.h

INCLUDEPATH += ../inc/
INCLUDEPATH += ../../rpag/inc/


#settings for or-tools lib:
QMAKE_CXXFLAGS += -fPIC -std=c++11 -Wno-deprecated

DEFINES += USE_SCIP
DEFINES += USE_CPLEX
DEFINES += USE_GUROBI
#DEFINES += USE_GLPK
#DEFINES += USE_CBC
#DEFINES += USE_CLP
#DEFINES += USE_GLOP

#or-tools libs (yes you really need all of them...):
LIBS += -llinear_solver -lbase -lprotobuf -lgflags -lCoinUtils -lClp -lOsiCbc -lOsi -lOsiClp -lCgl -lCbc -lutil -lglop -lCbcSolver

macx{
    message("Build for MAC.")
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9 #must be >= 10.9 (c++v11 support)

    #addjust these paths to you local installation of or-tools (see https://developers.google.com/optimization/):
    INCLUDEPATH += ../libs/or-tools-r3854/src
    LIBS += -L../libs/or-tools-r3854/lib
    LIBS += -L../libs/or-tools-r3854/dependencies/install/lib

    contains(DEFINES, USE_CPLEX){
        #addjust these paths to you local cplex installation (optional, see http://www.ibm.com/software/commerce/optimization/cplex-optimizer/):
        LIBS += -L/Users/pluto/apps/IBM/CPLEX_Studio126/cplex/lib/x86-64_osx/static_pic
        LIBS += -lcplex -lcplexdistmip -lilocplex
        message("Build with cplex optimizer")
    } else {
        message("Build without cplex optimizer")
    }
    contains(DEFINES, USE_GUROBI){
        #addjust these paths to you local gurobi installation (optional, see http://www.gurobi.com):
        LIBS += -L/Library/gurobi600/mac64/lib
        LIBS += -lgurobi_c++
        message("Build with gurobi optimizer")
    } else {
        message("Build without gurobi optimizer")
    }
}

linux-g++{
    message("Build for UNIX.")
    #addjust these paths to you local or-tools installation (see https://developers.google.com/optimization/):
    INCLUDEPATH += /opt/or-tools/src
    INCLUDEPATH += /opt/or-tools/dependencies/install/include/
    LIBS += -L/opt/or-tools/lib
    LIBS += -L/opt/or-tools/dependencies/install/lib
    LIBS += -lsplit -lglop

    #addjust these paths to you local cplex installation (optional, see http://www.ibm.com/software/commerce/optimization/cplex-optimizer/):
    LIBS += -L/opt/ibm/ILOG/CPLEX_Studio126/cplex/lib/x86-64_linux/static_pic
    LIBS += -lcplex -lcplexdistmip -lilocplex -lpthread -lrt

    #addjust these paths to you local gurobi installation (optional, see http://www.gurobi.com):
    LIBS += -L/opt/gurobi/gurobi600/linux64/lib
    LIBS += -lgurobi_c++ -lgurobi60
}

