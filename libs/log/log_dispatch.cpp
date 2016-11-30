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

#include <string.h>

#include "libs/log/log_dispatch.h"

using namespace Err;
using namespace Sys;
using namespace Log;

/******************************************************************************/
LogDispatch::LogDispatch() :
    _log_list(NULL),
    _num_logs(0)
{
}
/******************************************************************************/
LogDispatch::~LogDispatch()
{
    delete[] _log_list;
}
/******************************************************************************/
void LogDispatch::va_notice(const char *string, va_list args)
{
    for (unsigned i = 0; i < _num_logs; ++i)
	_log_list[i]->va_notice(string, args);
}
/******************************************************************************/
void LogDispatch::va_debug(unsigned level, const char *string, va_list args)
{
    for (unsigned i = 0; i < _num_logs; ++i)
	_log_list[i]->va_debug(level, string, args);
}
/******************************************************************************/
void LogDispatch::va_warning(const char *string, va_list args)
{
    for (unsigned i = 0; i < _num_logs; ++i)
	_log_list[i]->va_warning(string, args);
}
/******************************************************************************/
void LogDispatch::va_error(const char *string, va_list args)
{
    for (unsigned i = 0; i < _num_logs; ++i)
	_log_list[i]->va_error(string, args);
}
/******************************************************************************/
void LogDispatch::set_level(unsigned level)
{
    for (unsigned i = 0; i < _num_logs; ++i)
	_log_list[i]->set_level(level);
}
/******************************************************************************/
unsigned LogDispatch::get_level(void)
{
    unsigned	level = 0;

    for (unsigned i = 0; i < _num_logs; ++i)
	if (level < _log_list[i]->get_level())
	    level = _log_list[i]->get_level();

    return level;
}
/******************************************************************************/
void LogDispatch::set_prefix(const char *prefix)
{
    for (unsigned i = 0; i < _num_logs; ++i)
	_log_list[i]->set_prefix(prefix);
}
/******************************************************************************/
Err::Error LogDispatch::add_log(LogInterface &log)
{
    unsigned            new_length = _num_logs + 1;
    LogInterface * *    new_list   = new LogInterface *[new_length];

    memcpy(new_list, _log_list, _num_logs * sizeof(LogInterface *));

    new_list[_num_logs] = &log;

    _num_logs = new_length;
    _log_list = new_list;

    return success;
}
/******************************************************************************/
void LogDispatch::clear()
{
    _num_logs = 0;
    _log_list = NULL;
}
/******************************************************************************/
