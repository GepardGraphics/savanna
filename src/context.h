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

#ifndef CONTEXT_H
#define CONTEXT_H

#include <iostream>
#include API_INCLUDE

namespace savanna {

typedef CONTEXT_API BaseContext;
typedef CANVAS_API BaseCanvas;

class Context : BaseContext {
public:
    Context(BaseCanvas& canvas) : BaseContext(canvas)
    {
        std::cerr << "Construct." << std::endl;
    }
    ~Context()
    {
       std::cerr << "Destruct." << std::endl;
    }

    void setFillColor(const float red, const float green, const float blue, const float alpha = 1.0f);
    void fillRect(const float x, const float y, const float width, const float height);
};

class Canvas {
public:
    Canvas(uint32_t width_, uint32_t height_)
        : width(width_)
        , height(height_)
        , _canvas(width_, height_)
        , _context(new Context(_canvas))
    {
    }
    ~Canvas()
    {
        if (_context)
            delete _context;
    }

    Context& getContext() { return *_context; }
    BaseCanvas& getCanvas() { return _canvas; }

    uint32_t width;
    uint32_t height;
private:

    BaseCanvas _canvas;
    Context* _context;
};

} // namespace savanna

#endif // CONTEXT_H
