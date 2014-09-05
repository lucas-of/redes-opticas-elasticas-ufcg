TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \ 
    Conexao.cpp \
    Def.cpp \
    General.cpp \
    Heuristics.cpp \
    main_EONsimulator.cpp \
    Route.cpp \
    node.cpp

OTHER_FILES += \
    Topology.txt \
    README.md

HEADERS += \
    Conexao.h \
    Def.h \
    General.h \
    Heuristics.h \
    Route.h \
    Main_Auxiliar.h \
    Event.h \
    node.h

