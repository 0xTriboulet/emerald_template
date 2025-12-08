/*
 * Copyright 2025 Raphael Mudge, Adversary Fan Fiction Writers Guild
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// The original copyright notice above. Some modifications made for exe debugging

#include "../tcg/libtcg/src/loaderdefs.h"
#include "../tcg/libtcg/src/tcg.h"
#include <stdio.h>

void dprintf(char * format, ...) {
	int    len;
	char * temp;

	va_list args;

	/* figure out the length of our buffer */
	va_start(args, format);
	len  = vsnprintf(NULL, 0, format, args);
	va_end(args);

	/* allocate our memory */
	temp = VirtualAlloc(NULL, len + 1, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	if (temp == NULL)
		return;

	/* format everything */
	va_start(args, format);
	vsnprintf(temp, len + 1, format, args);
	va_end(args);

	/* print it */
	OutputDebugStringA(temp);

	/* free our memory and move on with our lives */
	VirtualFree(temp, 0, MEM_RELEASE);
}
