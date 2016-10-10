SOURCES=	fill-rect-01-simple.cpp \
			fill-rect-02-simple.cpp \

EXECUTABLES=$(SOURCES:.cpp=)

all: $(EXECUTABLES)

$(EXECUTABLES):%:%.cpp make-bin-dir
	g++ -std=c++11 -pedantic -Wall -o ./bin/$@ $< -I./../../surfaces -lX11 -lEGL -lGLESv2

make-bin-dir:
	mkdir -p bin

clean:
	rm -rf bin
