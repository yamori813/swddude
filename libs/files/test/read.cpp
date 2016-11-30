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

#include "libs/error/error.h"
#include "libs/log/log.h"
#include "libs/files/path.h"
#include "libs/files/reader.h"

#define READER_DATA_FILE	"libs/files/test/read.inc"
#define READER_DATA_NAME	TestData

#include "libs/files/data.h"
#include "libs/files/data.cpp"

using namespace Err;
using namespace Log;
using namespace Files;

/******************************************************************************/
Error read_test()
{
    typedef CharacterReader::Char	Char;

    Path		file("test.rc");
    CharacterReader	reader(TestData::char_table,
			       TestData::char_count,
			       TestData::c_eof,
			       TestData::c_invalid);

    Check(reader.open(file));

    for (Char c = Char(0, TestData::c_bol);
	 c.type != TestData::c_eof;
	 c = reader.next_character())
	printf("Char: %c (0x%08x)\n", c.value, c.type);

    return success;
}
/******************************************************************************/
int main(int argc, char **argv)
{
    Error	check_error;

    CheckCleanup(read_test(), failure);

    return 0;

  failure:
    Err::stack()->print();
    return 1;
}
/******************************************************************************/
