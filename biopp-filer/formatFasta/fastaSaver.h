/*
fasta-saver.h: load and save sequences(NucSequence, PseudonucSequence, and AminoSequence)
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

template<class Sequence>
class FastaSaver
{
private:

    std::ofstream os;
    static const unsigned int lineLimit = 50;

    void saveSequence(const Sequence& seq);
    void saveDescription(const std::string& des);

public:

    FastaSaver(const std::string& file_name)
        : os(file_name.c_str())
    {
        if (!os.is_open()) //necesario?
            throw FileNotFound(file_name);
    }

    ~FastaSaver()
    {
        os.close();
    }

    void saveNextSequence(const std::string& title, const Sequence& seq);
    void saveNextSequence(const Sequence& seq);
};


template<class Sequence>
void FastaSaver<Sequence>::saveNextSequence(const std::string& title, const Sequence& seq)
{
    saveDescription(title);
    saveSequence(seq);
}

template<class Sequence>
void FastaSaver<Sequence>::saveNextSequence(const Sequence& seq)
{
    os << std::endl;
    saveSequence(seq);
}

template<class Sequence>
void FastaSaver<Sequence>::saveSequence(const Sequence& seq)
{
    const std::string sequence = seq.getString();
    unsigned int currentCharNumber(0);

    for (size_t i = 0; i < sequence.size(); i++)
    {
        os << sequence[i];
        if (++currentCharNumber == lineLimit)
        {
            os << std::endl;
            currentCharNumber = 0;
        }
    }

    if (currentCharNumber != lineLimit)
        os << std::endl;
}

template<class Sequence>
void FastaSaver<Sequence>::saveDescription(const std::string& title)
{
    os << ">" << title << std::endl;
}

}
#endif
