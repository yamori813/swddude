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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "libs/files/reader.h"

using namespace Err;
using namespace Files;

/******************************************************************************/
Reader::Reader() :
    _path(Path(String::empty())),
    _line(String::empty()),
    _line_number(0),
    _file(NULL),
    _offset(0)
{
}
/******************************************************************************/
Reader::Reader(Path path) :
    _path(path),
    _line(String::empty()),
    _line_number(0),
    _file(fopen((const char *)(path.get()), "r")),
    _offset(0)
{
}
/******************************************************************************/
Reader::Reader(FILE * file) :
    _path(String::empty()),
    _line(String::empty()),
    _line_number(0),
    _file(file),
    _offset(0)
{
}
/******************************************************************************/
Reader::~Reader()
{
    if (_file != NULL)
	fclose(_file);
}
/******************************************************************************/
Error Reader::open(Path path)
{
    _path        = path;
    _line        = String::empty();
    _line_number = 0;
    _file        = fopen((const char *)(path.get()), "r");
    _offset      = 0;

    CheckStringB(_file != NULL, "Failed to open file \"%s\".",
		 (const char *)(path.get()));

    return success;
}
/******************************************************************************/
Error Reader::open(FILE * file)
{
    _path        = String::empty();
    _line        = String::empty();
    _line_number = 0;
    _file        = file;
    _offset      = 0;

    return success;
}
/******************************************************************************/
String Reader::get_line(void)
{
    char	*data  = NULL;
    size_t	length = 0;
    ssize_t	count;

    if (_file == NULL || (count = getline(&data, &length, _file)) < 0)
    {
	_line = String::empty();
    }
    else
    {
	_line = String(data, true, true);

	_line_number += 1;
	_offset      += count;
    }

    return _line;
}
/******************************************************************************/
bool Reader::eof(void) const
{
    if (_file != NULL)
	return feof(_file);
    else
	return true;
}
/******************************************************************************/
String Reader::get_last_line(void) const
{
    return _line;
}
/******************************************************************************/
String Reader::get_location(void) const
{
    return String::format("%s : %d", (const char *)(_path.get()), _line_number);
}
/******************************************************************************/
unsigned Reader::get_offset(void) const
{
    return _offset;
}
/******************************************************************************/
unsigned Reader::get_line_number(void) const
{
    return _line_number;
}
/******************************************************************************/
Path Reader::get_path(void) const
{
    return _path;
}
/******************************************************************************/
CharacterReader::Char::Char(char value,
			    uint32_t type) :
    value(value),
    type(type)
{
}
/******************************************************************************/
CharacterReader::Block::Block() :
    data(new char[256])
{
}
/******************************************************************************/
CharacterReader::Block::~Block()
{
    delete[] data;
}
/******************************************************************************/
CharacterReader::CharacterReader() :
    _count(0),
    _eof(0),
    _invalid(0),
    _path(Path(String::empty())),
    _file(-1),
    _current(0xffffffff)
{
    memset(_table, 0, sizeof(_table));
}
/******************************************************************************/
CharacterReader::CharacterReader(CharData const * data,
                                 unsigned count,
                                 uint32_t eof,
                                 uint32_t invalid) :
    _data(data),
    _count(count),
    _eof(eof),
    _invalid(invalid),
    _path(Path(String::empty())),
    _file(-1),
    _current(0xffffffff)
{
    populate_table();
}
/******************************************************************************/
CharacterReader::~CharacterReader()
{
    if (_file > 0)
	close(_file);
}
/******************************************************************************/
Error CharacterReader::initialize(CharData const * data,
                                  unsigned count,
                                  uint32_t eof,
                                  uint32_t invalid)
{
    if (_file > 0)
	close(_file);

    _data    = data;
    _count   = count;
    _eof     = eof;
    _invalid = invalid;
    _path    = Path(String::empty());
    _file    = -1;
    _current = 0xffffffff;

    populate_table();

    return success;
}
/******************************************************************************/
void CharacterReader::populate_table()
{
    for (unsigned i = 0; i < 128; ++i)
	_table[i] = _data[0].type;

    for (unsigned i = 0; i < _count; ++i)
    {
	for (unsigned j = 0; _data[i].elements[j] != '\0'; ++j)
	{
	    unsigned	index = _data[i].elements[j];

	    if (index < 128)
		_table[index] = _data[i].type;
	}
    }
}
/******************************************************************************/
Error CharacterReader::open(Path path)
{
    Error	check_error;

    _path    = path;
    _current = 0;

    CheckCleanupStringP(_file = ::open((const char *)(path.get()), O_RDONLY),
			open_failed,
			"Failed to open file \"%s\".",
			(const char *)(path.get()));

    read_block(0x000);
    read_block(0x100);

    return success;

  open_failed:
    _path = Path(String::empty());

    return check_error;
}
/******************************************************************************/
bool CharacterReader::eof(void)
{
    const unsigned	index        = (_current >> 8) & 0x1;
    const unsigned	block_offset = _current & ~0xff;
    const unsigned	block_byte   = _current & 0xff;

    if (block_offset != _block[index].offset)
	read_block(block_offset);

    return (block_byte > _block[index].length);
}
/******************************************************************************/
void CharacterReader::read_block(uint32_t offset)
{
    const unsigned	index = (offset >> 8) & 0x1;

    _block[index].offset = lseek(_file, offset, SEEK_SET);
    _block[index].length = read(_file, _block[index].data, 256);
}
/******************************************************************************/
CharacterReader::Char CharacterReader::next_character()
{
    const unsigned	index        = (_current >> 8) & 0x1;
    const unsigned	block_offset = _current & ~0xff;
    const unsigned	block_byte   = _current & 0xff;

    if (block_offset != _block[index].offset)
	read_block(block_offset);

    if (block_byte < _block[index].length)
    {
	++_current;

	unsigned char   value = _block[index].data[block_byte];
	uint32_t        type  = (value < 128) ? _table[value] : _invalid;

	return Char(value, type);
    }
    else
    {
	return Char(0, _eof);
    }
}
/******************************************************************************/
String CharacterReader::get_string(uint32_t offset, uint32_t length)
{
    String	result;
    uint32_t	saved_current = _current;

    _current = offset;

    for (unsigned i = 0; i < length; ++i)
	result = result.append(next_character().value);

    _current = saved_current;

    return result;
}
/******************************************************************************/
void CharacterReader::set_offset(uint32_t offset)
{
    _current = offset;
}
/******************************************************************************/
uint32_t CharacterReader::get_offset() const
{
    return _current;
}
/******************************************************************************/
