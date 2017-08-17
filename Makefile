GLEWLIB= -lGLEW

OS := $(shell uname)
ifeq ($(OS), Darwin)
# Run MacOS commands
OPENGLLIB= -framework OpenGL
GLFWLIB = -lglfw
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
else
# check for Linux and run other commands
OPENGLLIB= -lGL
GLFWLIB = -lglfw3
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB) -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl
endif

LDFLAGS=-L/usr/local/lib 
CPPFLAGS=-I/usr/local/include -g

BIN=cg_ichi
SRC_MAIN=cg_ichi.cpp 

INCLUDES_FW=gl_framework.hpp
SRC_FW=gl_framework.cpp 
OBJ_FW=gl_framework.o

INCLUDES_SU=shader_util.hpp
SRC_SU=shader_util.cpp 
OBJ_SU=shader_util.o

all: $(BIN)

$(OBJ_FW): $(INCLUDES_FW) $(SRC_FW)
	g++ -c $(CPPFLAGS) $(SRC_FW) $(LDFLAGS) $(LIBS)

$(OBJ_SU): $(INCLUDES_SU) $(SRC_SU)
	g++ -c $(CPPFLAGS) $(SRC_SU) $(LDFLAGS) $(LIBS)

$(BIN): $(OBJ_FW) $(OBJ_SU) $(SRC_MAIN)
	g++ $(CPPFLAGS) $(SRC_MAIN) $(OBJ_FW) $(OBJ_SU) -o $(BIN) $(LDFLAGS) $(LIBS)

clean:
	rm -f *~ *.o $(BIN)
