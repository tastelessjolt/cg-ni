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
CPPFLAGS=-I/usr/local/include

BIN=cg_ichi
SRCS=cg_ichi.cpp gl_framework.cpp shader_util.cpp 
INCLUDES=gl_framework.hpp shader_util.hpp

all: $(BIN)

$(BIN): $(SRCS) $(INCLUDES)
	g++ $(CPPFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS) $(LIBS)

clean:
	rm -f *~ *.o $(BIN)
