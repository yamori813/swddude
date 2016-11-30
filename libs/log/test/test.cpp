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

#include <stdlib.h>

#include "libs/error/error.h"
#include "libs/sys/sys.h"
#include "libs/log/log_default.h"
#include "libs/log/log_file.h"
#include "libs/log/log_html.h"

using namespace Err;
using namespace Sys;
using namespace Log;

/******************************************************************************/
int main(int argc, char **argv)
{
    LogFile	log_file;
    LogHTML	log_html;
    Err::Error	check_error;

    CheckCleanup(log_file.open("test.log", true), open_failed);
    CheckCleanup(log_html.open("test.html", true), open_failed);

    log().set_level(2);
    log().set_prefix("Test");
    log().add_log(log_file);
    log().add_log(log_html);

    notice("This is a test message 0x%0x8", 0x12345678);
    debug(0, "This is a test debug message %d", 100);
    debug(1, "This is a deep test debug message");
    debug(2, "This is a deeper test debug message");
    debug(3, "This message shouldn't show up");
    debug(2, "This debug message takes\n"
	  "Multiple lines");
    notice("This notice also\n"
	   "takes a number\n"
	   "of lines.");
    notice("This is a notice from the logging library that should probably be wrapped because it is longer than "
	   "most screens are wide.  It is possible that someone has a screen this wide, but its not likely.");
    warning("This is a test warning %s", "(plus a string)");
    notice("And here-we-have-a-message-that-attempts-to-trigger-the-failure-mode-of-the-line-wrapper-by-having-"
	   "a-sequence-of-characters-with-no-breaks-so-the-wrapper-has-no-where-to-break-the-line.  I hope it works.");
    error("This is a test error (%d)", 15);

    return 0;

  open_failed:
    Err::stack()->print();
    return 1;
}
/******************************************************************************/
