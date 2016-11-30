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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "libs/log/log_file.h"

using namespace Err;
using namespace Sys;
using namespace Log;

#if defined(__APPLE__)
/******************************************************************************/
int vdprintf(int fd, const char * format, ...)
{
	return 0;
}
/******************************************************************************/
int dprintf(int fd, const char * format, ...)
{
	return 0;
}
#endif

/******************************************************************************/
LogFile::LogFile() :
    _level(0),
    _prefix(NULL),
    _block_length(0),
    _file(-1)
{
    char	*new_prefix = new char[1];

    new_prefix[0] = '\0';

    _prefix = new_prefix;
}
/******************************************************************************/
LogFile::~LogFile()
{
    delete[] _prefix;

    if (_file >= 0)
	::close(_file);
}
/******************************************************************************/
void LogFile::va_notice(const char *string, va_list args)
{
    prefix();
    block("Notice..: ");
    write(string, args);
    write("\n");
}
/******************************************************************************/
void LogFile::va_debug(unsigned level, const char *string, va_list args)
{
    if (level <= _level)
    {
	prefix();
	block("Debug...: %*s", level * 2, "");
	write(string, args);
	write("\n");
    }
}
/******************************************************************************/
void LogFile::va_warning(const char *string, va_list args)
{
    prefix();
    block("Warning.: ");
    write(string, args);
    write("\n");
}
/******************************************************************************/
void LogFile::va_error(const char *string, va_list args)
{
    prefix();
    block("Error...: ");
    write(string, args);
    write("\n");
}
/******************************************************************************/
Err::Error LogFile::open(const char *filename, bool overwrite)
{
    int	flags = O_WRONLY | O_CREAT;
    int	mode  = 00666;

    if (overwrite)	flags |= O_TRUNC;
    else		flags |= O_EXCL;

    CheckP(_file = ::open(filename, flags, mode));

    return Err::success;
}
/******************************************************************************/
void LogFile::prefix(void)
{
    if (strlen(_prefix))
	_block_length = dprintf(_file, "%s ", _prefix);
    else
	_block_length = 0;
}
/******************************************************************************/
void LogFile::block(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    block(string, args);
    va_end(args);
}
/******************************************************************************/
void LogFile::block(const char *string, va_list args)
{
    _block_length += vdprintf(_file, string, args);
}
/******************************************************************************/
void LogFile::write(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    write(string, args);
    va_end(args);
}
/******************************************************************************/
void LogFile::write(const char *string, va_list args)
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
	dprintf(_file, "%s\n", output);
	output = end + 1;

	if (*output != '\0')
	    dprintf(_file, "%*s", _block_length, "");
    }

    dprintf(_file, "%s", output);
    free(allocated);
}
/******************************************************************************/
void LogFile::set_level(unsigned level)
{
    _level = level;
}
/******************************************************************************/
unsigned LogFile::get_level(void)
{
    return _level;
}
/******************************************************************************/
void LogFile::set_prefix(const char *prefix)
{
    char	*new_prefix = new char[strlen(prefix) + 1];

    strcpy(new_prefix, prefix);

    delete[] _prefix;

    _prefix = new_prefix;
}
/******************************************************************************/
const char *LogFile::get_prefix(void)
{
    return _prefix;
}
/******************************************************************************/
