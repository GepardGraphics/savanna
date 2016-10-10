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

#ifndef FILL_RECT_01_SIMPLE_H
#define FILL_RECT_01_SIMPLE_H

// Include own X11Surface.
#include "./../../surfaces/x11-surface.h"

// EGL & GLES2 includes
#define GL_GLEXT_PROTOTYPES 1
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <string>

namespace practices {

// FillRectContext API

class FillRectContext {
public:
    FillRectContext(X11Surface& surface);
    ~FillRectContext();

    void setFillColor(const float red, const float green, const float blue, const float alpha);
    void fillRect(const float x, const float y, const float width, const float height);

private:
    bool initEGLContext();
    void destroyEGLContext();
    GLuint compileShaderPrograms(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

    X11Surface& _surface;

    EGLDisplay _eglDisplay;
    EGLSurface _eglSurface;
    EGLContext _eglContext;

    GLuint _program;

    float _red, _green, _blue, _alpha;
};

} // namespace practices

#endif // FILL_RECT_01_SIMPLE_H
