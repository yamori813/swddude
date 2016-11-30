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

#ifndef __libs_files_data_h__
#define __libs_files_data_h__

#include "libs/files/reader.h"

class READER_DATA_NAME
{
    typedef Files::CharacterReader::CharData	CharData;

public:
    /**************************************************************************
     * The CharacterNameBit enum is used to generate the unique bitfields for
     * the CharacterName enum.  If there are more that 32 CLASS invocations
     * (and thus more CLASS'es than bits in a uint32 then the construction of
     * the CharacterName enum will generate errors because the left shift
     * operator will overflow the enum's int.
     */
    #define CLASS(name, elements)		c_bit_##name,

    enum CharacterNameBit
    {
        #include "libs/files/data.wrap"
    };

    /**************************************************************************
     * Character classes and composites are both included in the CharacterName
     * enum.  The composites are or'ed collections of the class bitfields.
     */
    #define CLASS(name, elements)		c_##name = (1 << c_bit_##name),
    #define COMPOSITE(name, value)		c_##name = value,

    enum CharacterName
    {
        #include "libs/files/data.wrap"
    };
    /**************************************************************************/

    static const CharData	char_table[];
    static const unsigned	char_count;
};

#endif //__libs_files_data_h__
