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

#include "./../../src/context.h"
#include <stdlib.h>
#include <limits>

int main()
{
    const uint32_t width = 640u;
    const uint32_t height = 480u;
    const uint32_t numberOfRects = std::numeric_limits<uint32_t>::max();
    const int64_t fiveSecInMiliSec = 5000;
    int ret = 0;

    std::srand(1985);

    savanna::Canvas canvas(width, height);
    savanna::Context& ctx = canvas.getContext();
    ctx.measures.fillRect.benchmarkName = __FILE__;
    ctx.measures.fillRect.on = true;

    ctx.setFillColor(0.0f, 1.0f, 0.0f, 1.0f);
    auto begin = std::chrono::high_resolution_clock::now();
    for (uint32_t i = 0; i < numberOfRects; ++i) {
        ctx.fillRect(std::rand() % width, std::rand() % height, std::rand() % width, std::rand() % height);

        XEvent event;
        if (XCheckWindowEvent(canvas.getCanvas().display(), canvas.getCanvas().window(), KeyPress | ClientMessage, &event)) {
            if (event.type == KeyPress && XLookupKeysym(&event.xkey, 0) == XK_Escape) {
                ret = -1;
                break;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(end-begin).count() > fiveSecInMiliSec) {
            ret = -2;
            break;
        }
    }

    std::clog << ctx.measures.fillRect << ((ret == -2) ? " (exit timeout)" : ((ret == -1) ? " (interrupted)" : " done")) << std::endl;

    return ret;
}
