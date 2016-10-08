/* Copyright (C) 2016, Gepard Graphics
 * Copyright (C) 2016, Szilard Ledan <szledan@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * \todo description missing
 */

#include <string>

#include <iostream>
#define CRASH(X) \
    do { \
        std::cerr << X << std::endl; \
        abort(); \
    } while (false)

// X11 includes
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// EGL & GLES2 includes
#define GL_GLEXT_PROTOTYPES 1
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <vector> // only for logging error messages of shader compilation

namespace practices {

// X11Surface

class X11Surface {
public:
    X11Surface(const uint32_t width = 640, const uint32_t height = 480, const std::string windowTitle = __FILE__)
        : _width(width)
        , _height(height)
    {
        _display = XOpenDisplay(NULL);

        if (_display) {
            XSetWindowAttributes windowAttributes;
            XWMHints hints;
            Window root = 0;

            root = XDefaultRootWindow(_display);
            windowAttributes.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
            _window = XCreateWindow ( // create a window with the provided parameters
                _display, root, 0, 0, width, height, 0,
                CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &windowAttributes);
            hints.input = True;
            hints.flags = InputHint;
            XSetWMHints(_display, _window, &hints);

            XMapWindow(_display, _window); // make the window visible on the screen
            XStoreName(_display, _window, windowTitle.c_str());
        }
    }
    ~X11Surface()
    {
        if (_display) {
            XDestroyWindow(_display, _window);
            XCloseDisplay(_display);
        }
    }

    const uint32_t width() const { return _width; }
    const uint32_t height() const { return _height; }
    Display* display() { return _display; }
    Window window() { return _window; }

private:
    uint32_t _width;
    uint32_t _height;
    Display* _display;
    Window _window;
};

static const std::string s_fillRectVertexShader = "\
        uniform vec2 in_size;                           \
                                                        \
        attribute vec2 in_position;                     \
        attribute vec4 in_color;                        \
                                                        \
        varying vec4 v_color;                           \
                                                        \
        void main(void)                                 \
        {                                               \
            v_color = in_color;                         \
            vec2 coords = (2.0 * in_position.xy / in_size.xy) - 1.0; \
            coords.y = -coords.y;                       \
            gl_Position = vec4(coords, 1.0, 1.0);       \
        }                                               \
                                     ";

static const std::string s_fillRectFragmentShader = "\
        precision highp float;                          \
                                                        \
        varying vec4 v_color;                           \
                                                        \
        void main(void)                                 \
        {                                               \
            gl_FragColor = v_color;                     \
        }                                               \
                                       ";

// FillRectContext API

class FillRectContext {
public:
    FillRectContext(X11Surface& surface)
        : _surface(surface)
        , _red(0.0f), _green(0.0f), _blue(0.0f), _alpha(0.0f)
    {
        if (initEGLContext()) {
            _program = compileShaderPrograms(s_fillRectVertexShader, s_fillRectFragmentShader);
        }
    }
    ~FillRectContext()
    {
        if (_program)
            glDeleteProgram(_program);
        destroyEGLContext();
    }

    void setFillColor(const float red, const float green, const float blue, const float alpha)
    {
        _red = red;
        _green = green;
        _blue = blue;
        _alpha = alpha;
    }
    void fillRect(const float x, const float y, const float width, const float height)
    {
        const int numberOfTriangles = 2;
        const GLubyte indices[] = {
            0, 1, 2, // first triangle
            1, 2, 3, // second triangle
        };

        const int numberOfAVertexAttributes = 2 + 4;
        const GLfloat vertexAttributes[] = {
            GLfloat(x), GLfloat(y), // top-left coords
            GLfloat(_red), GLfloat(_green), GLfloat(_blue), GLfloat(_alpha), // color of top-left vertex

            GLfloat(x), GLfloat(y + height), // bottom-left coords
            GLfloat(_red), GLfloat(_green), GLfloat(_blue), GLfloat(_alpha), // color of top-left vertex

            GLfloat(x + width), GLfloat(y), // top-right coords
            GLfloat(_red), GLfloat(_green), GLfloat(_blue), GLfloat(_alpha), // color of top-left vertex

            GLfloat(x + width), GLfloat(y + height), // bottom-right coords
            GLfloat(_red), GLfloat(_green), GLfloat(_blue), GLfloat(_alpha), // color of top-left vertex
        };

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(_program);

        {
            GLuint index = glGetUniformLocation(_program, "in_size");
            glUniform2f(index, _surface.width(), _surface.height());
        }

        const GLsizei stride = numberOfAVertexAttributes * sizeof(GL_FLOAT);
        int offset = 0;
        {
            const GLint size = 2;
            GLuint index = glGetAttribLocation(_program, "in_position");
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, vertexAttributes + offset);
            offset = size;
        }

        {
            const GLint size = 4;
            GLuint index = glGetAttribLocation(_program, "in_color");
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, vertexAttributes + offset);
        }

        glDrawElements(GL_TRIANGLES, numberOfTriangles * 3, GL_UNSIGNED_BYTE, indices);

        eglSwapBuffers(_eglDisplay, _eglSurface);
    }
private:

    bool initEGLContext()
    {
        const EGLint configAttribs[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 8,
            EGL_STENCIL_SIZE, 8,

            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_SAMPLE_BUFFERS, 0,
            EGL_SAMPLES, 0,
            EGL_NONE
        };

        const EGLint contextAttribs[] = {
            // Identify OpenGL 2 ES context
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE,
        };

        EGLDisplay eglDisplay = 0;
        EGLSurface eglSurface = 0;
        EGLint numOfConfigs = 0;
        EGLConfig eglConfig = 0;

        if (_surface.display()) {
            // Initialize EGL.
            eglDisplay = eglGetDisplay((EGLNativeDisplayType) _surface.display());

            if (eglDisplay == EGL_NO_DISPLAY) {
                CRASH("eglGetDisplay returned EGL_DEFAULT_DISPLAY");
            }
            if (eglInitialize(eglDisplay, NULL, NULL) != EGL_TRUE) {
                CRASH("eglInitialize returned with EGL_FALSE");
            }
            if (eglChooseConfig(eglDisplay, configAttribs, &eglConfig, 1, &numOfConfigs) != EGL_TRUE) {
                CRASH("eglChooseConfig returned with EGL_FALSE");
            }
            EGLNativeWindowType eglWindow = _surface.window();
            eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglWindow, NULL);
            if (eglSurface == EGL_NO_SURFACE) {
                CRASH("eglCreateWindowSurface returned EGL_NO_SURFACE");
            }
            _eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribs);
            if (_eglContext == EGL_NO_CONTEXT) {
                CRASH("eglCreateContext returned EGL_NO_CONTEXT");
            }
            if (eglMakeCurrent(eglDisplay, eglSurface, eglSurface, _eglContext) != EGL_TRUE) {
                CRASH("eglMakeCurrent returned EGL_FALSE");
            }

            // Set EGL display & surface.
            _eglDisplay = eglDisplay;
            _eglSurface = eglSurface;

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Set and clear viewport.
        const GLfloat red = 0.0f;
        const GLfloat green = 0.0f;
        const GLfloat blue = 0.0f;
        const GLfloat alpha = 0.0f;
        glClearColor(red, green, blue, alpha);
        glViewport(0, 0, _surface.width(), _surface.height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set blending mode.
        glEnable(GL_BLEND);

        return true;
    }
    void destroyEGLContext()
    {
        eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(_eglDisplay, _eglContext);
        eglDestroySurface(_eglDisplay, _eglSurface);
        eglTerminate(_eglDisplay);
    }
    GLuint compileShaderPrograms(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
    {
        GLuint vertexShader = 0;
        {
            const GLchar* shaderSource = (const GLchar*)vertexShaderSource.c_str();
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &shaderSource, NULL);
            glCompileShader(vertexShader);

            GLint isCompiled;
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);

            if (isCompiled != GL_TRUE) {
                GLint maxLength = 0;
                glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> errorLog(maxLength);
                GLsizei length = 0;
                glGetShaderInfoLog(vertexShader, maxLength, &length, errorLog.data());

                std::cerr << "Shader compilation failed with: " << std::string(errorLog.begin(), errorLog.end()) << std::endl;
            }
        }

        GLuint fragmentShader = 0;
        {
            const GLchar* shaderSource = (const GLchar*)fragmentShaderSource.c_str();
            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &shaderSource, NULL);
            glCompileShader(fragmentShader);

            GLint isCompiled;
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);

            if (isCompiled != GL_TRUE) {
                GLint maxLength = 0;
                glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> errorLog(maxLength);
                GLsizei length = 0;
                glGetShaderInfoLog(fragmentShader, maxLength, &length, errorLog.data());

                std::cerr << "Shader compilation failed with: " << std::string(errorLog.begin(), errorLog.end()) << std::endl;
            }
        }

        const GLuint program = glCreateProgram();
        if (vertexShader && fragmentShader) {
            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);
            glLinkProgram(program);

            GLint isLinked;
            glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

            if (isLinked != GL_TRUE) {
                GLint maxLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> errorLog(maxLength);
                GLsizei length = 0;
                glGetProgramInfoLog(program, maxLength, &length, errorLog.data());

                std::cerr << "Shader program link failed with: " << std::string(errorLog.begin(), errorLog.end()) << std::endl;

                glDeleteProgram(program);
                return 0;
            }
        }

        // According to the specification, the shaders are kept
        // around until the program object is freed (reference counted).
        if (vertexShader) {
            glDeleteShader(vertexShader);
        }
        if (fragmentShader) {
            glDeleteShader(fragmentShader);
        }

        return program;
    }

    X11Surface& _surface;

    EGLDisplay _eglDisplay;
    EGLSurface _eglSurface;
    EGLContext _eglContext;

    GLuint _program;

    float _red, _green, _blue, _alpha;
};

} // namespace practices

#include <thread>
#include <chrono>

int main()
{
    practices::X11Surface x11Surface(640, 480);
    practices::FillRectContext ctx(x11Surface);

    ctx.setFillColor(1.0f, 0.0f, 0.0f, 1.0f);
    ctx.fillRect(70, 40, 300, 200);

    ctx.setFillColor(0.0f, 1.0f, 0.0f, 1.0f);
    ctx.fillRect(170, 140, 300, 200);

    ctx.setFillColor(0.0f, 0.0f, 1.0f, 1.0f);
    ctx.fillRect(270, 240, 300, 200);

    XEvent event;
    do {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));   // Only for CPU sparing.
        if (XCheckWindowEvent(x11Surface.display(), x11Surface.window(), KeyPress | ClientMessage, &event)) {
            if (event.type == KeyPress && XLookupKeysym(&event.xkey, 0) == XK_Escape) {
                break;
            }
        }
    } while (true);

    return 0;
}
