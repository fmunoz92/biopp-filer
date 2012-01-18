/*
fasta-parser.h: load and save sequences(NucSequence, PseudonucSequence, and AminoSequence)
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
#include "bioppFilerException.h"
#include "fsm.h"

DEFINE_SPECIFIC_EXCEPTION(FileNotFound, BioppFilerException);

template<class Sequence>
class FastaParser
{
private:
    const std::string file_name;

    static void removeComment(std::string& line)
    {
        const size_t comentPosistion = line.find_first_of(";");
        line = line.substr(0, comentPosistion);
    }

    static void removeFirstChar(std::string& line)
    {
        line = line.substr(1, line.size());
    }

    static void removeWhiteSpace(std::string& line)
    {
        line = mili::trim(line);
    }

    void stimulate(FastaMachine<Sequence>& fsm, std::string& line) const;

public:
    FastaParser(const std::string& file_name)
        : file_name(file_name)
    {}
    ~FastaParser()
    {}
    void load_sequence(std::string& title, Sequence& seq) const;
    void save_sequence(const std::string& title, const Sequence& seq) const;
};


template<class Sequence>
void FastaParser<Sequence>::stimulate(FastaMachine<Sequence>& fsm, std::string& line) const
{
    removeComment(line);
    removeWhiteSpace(line);

    if (line[0] == '>')
    {
        removeFirstChar(line);
        fsm.procesarLineaDescriptiva(line);
    }
    else if (line.size() == 0)
        fsm.procesarLineaVacia(line);
    else
        fsm.procesarLineaSecuencia(line);
}
    
template<class Sequence>
void FastaParser<Sequence>::load_sequence(std::string& title, Sequence& seq) const
{
    std::ifstream is(file_name.c_str());
    std::string line;
    FastaMachine<Sequence> fsm(seq, title);

    if (!is.is_open())
		throw FileNotFound(file_name);

    while (std::getline(is, line))
        stimulate(fsm, line);

    is.close();
}


template<class Sequence>
void FastaParser<Sequence>::save_sequence(const std::string& title, const Sequence& seq) const
{
    static const size_t lineLimit(50);
    const std::string sequence = seq.getString();
    int currentCharNumber(0);
    std::ofstream of(file_name.c_str());

    of << ">" << title << std::endl;

    for (size_t i = 0; i < sequence.size(); i++)
    {
        of << sequence[i];
        if (++currentCharNumber == lineLimit)
        {
            of << std::endl;
            currentCharNumber = 0;
        }
    }

    of.close();
}

#endif
