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

#ifndef __libs_files_reader_h__
#define __libs_files_reader_h__

#include "libs/error/error.h"
#include "libs/string/string.h"
#include "libs/files/path.h"

#include <stdio.h>
#include <stdint.h>

namespace Files
{
    class Reader
    {
	Path            _path;
	String          _line;
	int             _line_number;
	FILE *          _file;
	unsigned        _offset;

    public:
	Reader();
	explicit Reader(Path path);
	explicit Reader(FILE * file);
	~Reader(void);

	Err::Error open(Path path);
	Err::Error open(FILE * file);

	String   get_line(void);
	bool     eof(void)             const;
	String   get_last_line(void)   const;
	String   get_location(void)    const;
	unsigned get_offset(void)      const;
	unsigned get_line_number(void) const;
	Path     get_path(void)        const;
    };

    class CharacterReader
    {
    public:
	struct Char
	{
	    char	value;
	    uint32_t	type;

	    Char(char value, uint32_t type);
	};

	struct CharData
	{
	    uint32_t	type;
	    String	name;
	    String	elements;
	};

    private:
	typedef Err::Error	Error;

	struct Block
	{
	    uint32_t	offset;
	    uint32_t	length;
	    char	*data;

	    Block();
	    ~Block();
	};

	uint32_t                _table[128];
	CharData const *        _data;
	unsigned                _count;
	uint32_t                _eof;
	uint32_t                _invalid;
	Path                    _path;
	int                     _file;
	Block                   _block[2];
	uint32_t                _current;

	void read_block(uint32_t offset);
	void populate_table();

    public:
	CharacterReader();
	CharacterReader(CharData const * data,
                        unsigned count,
                        uint32_t eof,
                        uint32_t invalid);
	~CharacterReader();

	Error    initialize(CharData const * data,
                            unsigned count,
                            uint32_t eof,
                            uint32_t invalid);
	Error    open(Path path);
	bool     eof(void);
	Char     next_character();
	String   get_string(uint32_t offset, uint32_t length);
	void     set_offset(uint32_t offset);
	uint32_t get_offset() const;
    };
}

#endif //__libs_files_reader_h__
