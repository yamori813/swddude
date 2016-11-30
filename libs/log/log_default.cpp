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

#include <stdarg.h>

#include "libs/error/error.h"
#include "libs/sys/module.h"

#include "libs/log/log_default.h"
#include "libs/log/log_dispatch.h"
#include "libs/log/log_console.h"

using namespace Err;
using namespace Sys;

class LoggingModule : public Module
{
public:
    Log::LogDispatch	log_dispatch;
    Log::LogConsole	log_console;

    Err::Error initialize();
    Err::Error shutdown();

    const char *name() {return "Logging";}
    const char **dependancies() {static const char *dep[] = {NULL}; return dep;}
};

static LoggingModule	logging_module;
ModuleInitializer	logging_module_initializer(&logging_module);

/******************************************************************************/
Err::Error LoggingModule::initialize(void)
{
    Check(log_dispatch.add_log(log_console));

    return Err::success;
}
/******************************************************************************/
Err::Error LoggingModule::shutdown(void)
{
    return Err::success;
}
/******************************************************************************/
void Log::notice(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    logging_module.log_dispatch.va_notice(string, args);
    va_end(args);
}
/******************************************************************************/
void Log::debug(unsigned level, const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    logging_module.log_dispatch.va_debug(level, string, args);
    va_end(args);
}
/******************************************************************************/
void Log::warning(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    logging_module.log_dispatch.va_warning(string, args);
    va_end(args);
}
/******************************************************************************/
void Log::error(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    logging_module.log_dispatch.va_error(string, args);
    va_end(args);
}
/******************************************************************************/
Log::LogDispatch &Log::log()
{
    return logging_module.log_dispatch;
}
/******************************************************************************/
