QT += opengl
#LIBS+=-lopengl32 -lglu32 #window libs
LIBS+=-lglut -lGLU #linux ubuntu libs
HEADERS += \
    myglwidget.h

SOURCES += \
    myglwidget.cpp \
    main.cpp
