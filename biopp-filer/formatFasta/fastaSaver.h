/*
fastaSaver.h: load and save sequences(NucSequence, PseudonucSequence, and AminoSequence)
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

#ifndef FASTA_SAVER_H
#define FASTA_SAVER_H

#include <string>
#include <fstream>

namespace bioppFiler
{

template<class SequenceType>
class FastaSaver
{
public:

    inline FastaSaver(const std::string& file_name);

    inline void saveNextSequence(const std::string& title, const SequenceType& seq);
    inline void saveNextSequence(const SequenceType& seq);

private:

    std::ofstream os;
    static const unsigned int lineLimit = 50;

    inline void saveSequence(const SequenceType& seq);
    inline void saveDescription(const std::string& des);

};
}

#define FASTA_SAVER_INLINE_H
#include "fastaSaver_inline.h"
#undef FASTA_SAVER_INLINE_H
#endif
