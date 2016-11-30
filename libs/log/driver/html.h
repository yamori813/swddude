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

#ifndef __log_log_html_h__
#define __log_log_html_h__

#include "libs/error/error.h"
#include "libs/sys/sys.h"
#include "libs/log/log_interface.h"

namespace Log
{
    class LogHTML : public LogInterface
    {
    private:
	unsigned        _level;
	char const *    _prefix;
	unsigned        _block_length;
	int             _file;
	bool            _in_style;

	/*
	 * No copying of LogHTML objects
	 */
	LogHTML(const LogHTML &other);
	LogHTML &operator=(const LogHTML &other);

	void prefix(void);
	void block(const char *string, ...);
	void block(const char *string, va_list args);
	void color(Color color);
	void write(const char *string, ...);
	void write(const char *string, va_list args);

    public:
	LogHTML();
	virtual ~LogHTML();

	/*
	 * Implement the LogInterface.
	 */
	void va_notice (const char *string, va_list args);
	void va_debug  (unsigned level,
                        const char *string, va_list args);
	void va_warning(const char *string, va_list args);
	void va_error  (const char *string, va_list args);

	void set_level(unsigned level);
	unsigned get_level(void);
	void set_prefix(const char *prefix);

	/*
	 * These methods are specific to the LogConsole object.
	 */
	Err::Error open(const char *filename, bool overwrite);
	const char *get_prefix(void);
    };
}

#endif /*__log_log_html_h__*/
