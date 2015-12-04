QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
QMAKE_MAC_SDK = macosx10.11
  
QT += widgets printsupport

CONFIG += release
# CONFIG += static
CONFIG += -std=gnu++11
CONFIG += WARN_OFF
CONFIG += debug

QMAKE_CXXFLAGS += -std=c++11

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
sgsimulationhandler.hpp \
sgsimulationplot.hpp \
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
sgsimulationhandler.cpp \
sgsolutionhandler.cpp

LIBS += -L../lib/ -L/usr/local/lib/ # -L/opt/Qt/lib/ -L/opt/Qt/plugins/platforms/
linux-g++ {
LIBS += -Bstatic -lsg -Wl,-Bstatic -lboost_serialization -Wl,-Bdynamic #-lqxcb
}
macx {
LIBS += -lc++ -lsg /usr/local/lib/libboost_serialization.a 
}

PRE_TARGETDEPS=../lib/libsg.a
DESTDIR = ./build

MOC_DIR = ./tmp
OBJECTS_DIR = ./tmp

target.path = ./
INSTALLS += target
