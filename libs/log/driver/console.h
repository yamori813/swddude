/*
 * Copyright (c) 2010, Anton Staaf
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the project nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __log_log_console_h__
#define __log_log_console_h__

#include "libs/sys/sys.h"
#include "libs/log/log_interface.h"

namespace Log
{
    class LogConsole : public LogInterface
    {
    private:
	int write_prefix(const char *prefix);
	int write_block(const char *string, ...);
	int write_block(const char *string, va_list args);
	void write_color(Color color);
	void write(const char *string, ...);
	void write(const char *string, va_list args);

    public:
	LogConsole();
	virtual ~LogConsole();

	/*
	 * Implement the LogInterface.
	 */
	void va_notice (const char *prefix, const char *string, va_list args);
	void va_debug  (unsigned level,
                        const char *prefix, const char *string, va_list args);
	void va_warning(const char *prefix, const char *string, va_list args);
	void va_error  (const char *prefix, const char *string, va_list args);
    };
}

#endif /*__log_log_console_h__*/
