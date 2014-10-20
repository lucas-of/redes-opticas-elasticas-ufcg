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
    Node.cpp \
    Enlace.cpp \
    ASE_Noise.cpp

OTHER_FILES += \
    Topology.txt \
    README.md \
    Topology2.txt

HEADERS += \
    Conexao.h \
    Def.h \
    General.h \
    Heuristics.h \
    Route.h \
    Main_Auxiliar.h \
    Event.h \
    Node.h \
    Enlace.h \
    Constantes.h

    install_it.path = $$OUT_PWD
    install_it.files = Topology.txt Topology2.txt

INSTALLS += \
    install_it
