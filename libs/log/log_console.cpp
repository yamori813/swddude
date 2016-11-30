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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/log/log_console.h"

using namespace Log;

/******************************************************************************/
LogConsole::LogConsole() :
    _level(0),
    _prefix(NULL),
    _block_length(0)
{
    char	*new_prefix = new char[1];

    new_prefix[0] = '\0';

    _prefix = new_prefix;
}
/******************************************************************************/
LogConsole::~LogConsole()
{
    delete[] _prefix;
}
/******************************************************************************/
void LogConsole::va_notice(const char *string, va_list args)
{
    color(LogInterface::white);
    prefix();
    color(LogInterface::normal);
    block("Notice..: ");
    write(string, args);
    write("\n");
}
/******************************************************************************/
void LogConsole::va_debug(unsigned level, const char *string, va_list args)
{
    if (level <= _level)
    {
	color(LogInterface::white);
	prefix();
	color(LogInterface::blue);
	block("Debug...: %*s", level * 2, "");
	color(LogInterface::normal);
	write(string, args);
	write("\n");
    }
}
/******************************************************************************/
void LogConsole::va_warning(const char *string, va_list args)
{
    color(LogInterface::white);
    prefix();
    color(LogInterface::yellow);
    block("Warning.: ");
    color(LogInterface::normal);
    write(string, args);
    write("\n");
}
/******************************************************************************/
void LogConsole::va_error(const char *string, va_list args)
{
    color(LogInterface::white);
    prefix();
    color(LogInterface::red);
    block("Error...: ");
    color(LogInterface::normal);
    write(string, args);
    write("\n");
}
/******************************************************************************/
void LogConsole::prefix(void)
{
    _block_length = printf("%s ", _prefix);
}
/******************************************************************************/
void LogConsole::block(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    block(string, args);
    va_end(args);
}
/******************************************************************************/
void LogConsole::block(const char *string, va_list args)
{
    _block_length += vprintf(string, args);
}
/******************************************************************************/
void LogConsole::color(Color color)
{
    switch (color)
    {
    case black:   printf("%c[1;30m", 27); break;
    case red:     printf("%c[1;31m", 27); break;
    case green:   printf("%c[1;32m", 27); break;
    case yellow:  printf("%c[1;33m", 27); break;
    case blue:    printf("%c[1;34m", 27); break;
    case magenta: printf("%c[1;35m", 27); break;
    case cyan:    printf("%c[1;36m", 27); break;
    case white:   printf("%c[1;37m", 27); break;
    case normal:  printf("%c[0m", 27); break;
    }
}
/******************************************************************************/
void LogConsole::write(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    write(string, args);
    va_end(args);
}
/******************************************************************************/
void LogConsole::write(const char *string, va_list args)
{
    char	*allocated;
    char	*output;
    char	*end;

    if (vasprintf(&allocated, string, args) < 0)
	return;

    output = allocated;

    while ((end = index(output, '\n')) != NULL)
    {
	*end = '\0';
	printf("%s\n", output);
	output = end + 1;

	if (*output != '\0')
	    printf("%*s", _block_length, "");
    }

    printf("%s", output);
    free(allocated);
}
/******************************************************************************/
void LogConsole::set_level(unsigned level)
{
    _level = level;
}
/******************************************************************************/
unsigned LogConsole::get_level(void)
{
    return _level;
}
/******************************************************************************/
void LogConsole::set_prefix(const char *prefix)
{
    char	*new_prefix = new char[strlen(prefix) + 1];

    strcpy(new_prefix, prefix);

    delete[] _prefix;

    _prefix = new_prefix;
}
/******************************************************************************/
const char *LogConsole::get_prefix(void)
{
    return _prefix;
}
/******************************************************************************/
