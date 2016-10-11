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

#ifndef X11_SURFACE_H
#define X11_SURFACE_H

/*!
 * \todo description missing
 */

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>

namespace practices {

// X11Surface

class X11Surface {
public:
    X11Surface(const uint32_t width = 640, const uint32_t height = 480, const std::string windowTitle = "X to Savanna")
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

} // namespace practices

#endif // X11_SURFACE_H
