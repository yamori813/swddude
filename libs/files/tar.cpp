/*
 * Copyright (c) 2011, The Chromium OS Authors
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

#include "libs/files/tar.h"
#include "libs/types/static_assert.h"

#include <stdlib.h>

using namespace Files;

enum Type
{
    type_file_old	= '\0',
    type_file		= '0',
    type_hard_link	= '1',
    type_sym_link	= '2',
    type_char_dev	= '3',
    type_block_dev	= '4',
    type_directory	= '5',
    type_fifo		= '6',
    type_contiguous	= '7',
};

struct Tar::Header
{
    char	name[100];
    char	mode[8];
    char	owner_id[8];
    char	group_id[8];
    char	size[12];
    char	mtime[12];
    char	checksum[8];
    char	type;
    char	link[100];
    char	ustar[6];
    char	version[2];
    char	owner[32];
    char	group[32];
    char	dev_major[8];
    char	dev_minor[8];
    char	prefix[155];
    char	padding[12];
};

static_assert(sizeof(Tar::Header) == 512, tar_header_size_incorrect);

/******************************************************************************/
Tar::Tar(void const * data) :
    _data(data),
    _header(static_cast<Header const *>(data))
{
}
/******************************************************************************/
FILE * Tar::open(Path const & path)
{
    for (int index = 0; _header[index].name[0] != '\0'; ++index)
    {
	int	size = strtol(_header[index].size, NULL, 8);

	if (path.get().equal(_header[index].name))
	    return fmemopen((void *)(_header + index + 1), size, "r");

	index += (size + 511) >> 9;
    }

    return NULL;
}
/******************************************************************************/
