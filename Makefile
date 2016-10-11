BUILD_DIR=build/bin

all: make-bin-dir
	g++ -std=c++11 -pedantic -Wall \
	-DSAVANNA_BENCHMARK_ON -DCANVAS_API="practices::X11Surface" -DCONTEXT_API="practices::FillRectContext" -DAPI_INCLUDE="\"./../practices/fill-rect/gles2/fill-rect.h\"" \
	-o $(BUILD_DIR)/sbms \
	./benchmarks/fill-rect/fill-rect-01-one-rect.cpp ./practices/fill-rect/gles2/fill-rect-01-simple.cpp ./src/context.cpp \
	-lX11 -lEGL -lGLESv2

make-bin-dir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
