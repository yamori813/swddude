// -*- mode: c++ -*-
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

CLASS(invalid,      "")
CLASS(bol,          "")
CLASS(eof,          "")
CLASS(space,        " ")
CLASS(tab,          "\t")
CLASS(cr,           "\r")
CLASS(lf,           "\n")
CLASS(quote,        "\"")
CLASS(backslash,    "\\")
CLASS(slash,        "/")
CLASS(plus,         "+")
CLASS(minus,        "-")
CLASS(hash,         "#")
CLASS(period,       ".")
CLASS(left,         "(")
CLASS(right,        ")")
CLASS(left_square,  "[")
CLASS(right_square, "]")
CLASS(left_brace,   "{")
CLASS(right_brace,  "}")
CLASS(comma,        ",")
CLASS(colon,        ":")
CLASS(equal,        "=")
CLASS(underscore,   "_")
CLASS(semicolon,    ";")
CLASS(esc,          "nrt")
CLASS(punct,        "!$%&\'*<>?@^`|~")
CLASS(nonesc,       "abcdefghijklmopqsuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
CLASS(zero,         "0")
CLASS(nonzero,      "123456789")

COMPOSITE(letter,      (c_esc | c_nonesc))
COMPOSITE(escaped,     (c_quote | c_backslash | c_esc))
COMPOSITE(punctuation, (c_quote | c_backslash | c_semicolon | c_slash | c_plus | c_minus | c_hash | c_period | c_left | c_right | c_comma | c_colon | c_equal | c_underscore | c_left_square | c_right_square | c_punct))
COMPOSITE(constituent, (c_letter | c_underscore | c_minus))
COMPOSITE(sign,        (c_plus | c_minus))
COMPOSITE(digit,       (c_zero | c_nonzero))
COMPOSITE(character,   (c_letter | c_space | c_punctuation | c_digit))
