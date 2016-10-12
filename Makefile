BENCHMARKS=	fill-rect-01-one-color.cpp \
			fill-rect-02-more-color.cpp \

BINARIES=$(BENCHMARKS:.cpp=)
BUILD_DIR=build/bin

all: $(BINARIES)

$(BINARIES):%:./benchmarks/fill-rect/%.cpp make-bin-dir
	g++ -std=c++11 -pedantic -Wall \
	-o $(BUILD_DIR)/$@ \
	-DSAVANNA_BENCHMARK_ON -DCANVAS_API="practices::X11Surface" -DCONTEXT_API="practices::FillRectContext" -DAPI_INCLUDE="\"./../practices/fill-rect/gles2/fill-rect.h\"" \
	$< ./practices/fill-rect/gles2/fill-rect-01-simple.cpp ./src/context.cpp \
	-lX11 -lEGL -lGLESv2

make-bin-dir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
