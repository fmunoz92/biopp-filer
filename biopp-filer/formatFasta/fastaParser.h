/*
fastaParser.h: load and save sequences(NucSequence, PseudonucSequence, and AminoSequence)
    Copyright (C) 2012 Facundo Muñoz FuDePAN

    This file is part of Biopp-filer.

    Biopp-filer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Biopp-filer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Biopp-filer.  If not, see <http://www.gnu.org/licenses/>.

    NOTE: This file is in prototype stage, and is under active development.
*/

#ifndef FASTA_PARSER_H
#define FASTA_PARSER_H

#include <string>
#include <fstream>
#include <mili/mili.h>
#include "fastaMachine.h"

namespace bioppFiler
{

template<class SequenceType>
class FastaParser
{
public:

    FastaParser(const std::string& file_name)
        : is(file_name.c_str())
    {
        if (!is.is_open())
            throw FileNotFound(file_name);
    }

    bool getNextSequence(std::string& description, SequenceType& sequence);

private:

    void removeComment(std::string& line);
    void removeFirstChar(std::string& line);
    void removeWhiteSpace(std::string& line);

    void stimulateFastaMachine();
    bool getNextSequence(std::string& description, std::string& sequence);

    std::ifstream is;
    FastaMachine fsm;
};
}

#define FASTA_PARSER_INLINE_H
#include "fastaParser_inline.h"
#undef FASTA_PARSER_INLINE_H
#endif
