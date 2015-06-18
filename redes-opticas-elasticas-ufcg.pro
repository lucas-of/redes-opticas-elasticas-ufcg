TEMPLATE = app
CONFIG += console

SOURCES += \
    Conexao.cpp \
    Def.cpp \
    General.cpp \
    Heuristics.cpp \
    main_EONsimulator.cpp \
    Route.cpp \
    Node.cpp \
    Enlace.cpp \
    ASE_Noise.cpp \
    Metricas.cpp \
    RWA.cpp \
    PSR.cpp \
    Main_Auxiliar.cpp \
    NSGA2.cpp \
    Regeneradores.cpp

OTHER_FILES += \
    README.md \
    TopologyNFS.txt \
    TopologyPBell.txt \
    TopologyRed8.txt \
    TopologyRed4.txt \
    TopologyNFSMod.txt \
    TopologyTop1.txt \
    TopologyTop2.txt

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
    Constantes.h \
    RWA.h \
    PSR.h \
    NSGA2.h \
    Regeneradores.h

    install_it.path = $$OUT_PWD
    install_it.files = TopologyNFS.txt TopologyPBell.txt TopologyRed8.txt TopologyRed4.txt TopologyNFSMod.txt TopologyTop1.txt TopologyTop2.txt

INSTALLS += \
    install_it

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp
