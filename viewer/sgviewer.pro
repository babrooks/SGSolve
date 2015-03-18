QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9

QT += widgets printsupport

CONFIG += release
CONFIG += -std=gnu++11
CONFIG += WARN_OFF
CONFIG += debug

INCLUDEPATH += ./hpp/
INCLUDEPATH += /usr/local/include
INCLUDEPATH += ../src/hpp
INCLUDEPATH += /usr/local/lib/ 
HEADERS = sgmainwindow.hpp \
qcustomplot.h \
sggamehandler.hpp \
sgtablemodel.hpp \
sgtableview.hpp \
sgprobabilitytablemodel.hpp \
sgpayofftablemodel.hpp \
sgsolutionhandler.hpp \
sgsolverworker.hpp \
sgcustomplot.hpp \
Q_DebugStream.h

VPATH += ./cpp ./hpp
SOURCEPATH += ./cpp
SOURCES = main.cpp\
sgmainwindow.cpp \
qcustomplot.cpp \
sgcustomplot.cpp \
sggamehandler.cpp \
sgprobabilitytablemodel.cpp \
sgpayofftablemodel.cpp \
sgsolutionhandler.cpp

LIBS += -L../lib/ -L/usr/local/lib/ 
linux-g++ {
LIBS += -Bstatic -lsg -Wl,-Bstatic -lboost_serialization -Wl,-Bdynamic
}
macx {
LIBS += -lc++ -lsg /usr/local/lib/libboost_serialization.a 
}

DESTDIR = ./build

MOC_DIR = ./tmp
OBJECTS_DIR = ./tmp

target.path = ./
INSTALLS += target
