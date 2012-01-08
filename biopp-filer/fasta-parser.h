/*
fasta-parser.h: load and save sequences(NucSequence, PseudonucSequence, and AminoSequence)
    Copyright (C) 2012 Facundo Muñoz FuDePAN

    This file is part of Biopp-filer.

    Aso is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Biopp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Biopp.  If not, see <http://www.gnu.org/licenses/>.

    NOTE: This file is in prototype stage, and is under active development.
*/

#ifndef FASTA_PARSER_H
#define FASTA_PARSER_H

#include "fsm.h"
#include <string>
#include <fstream>
#include <mili/mili.h>

template<class Sequence>
class FastaParser
{
private:
    const std::string file_name;

    enum LineType {lineaSecuencia, lineaDescriptiva, lineaVacia};

    static void removeComent(std::string& line)
    {
        size_t comentPosistion = line.find_first_of(";");
        line = line.substr(0, comentPosistion);
    }

    static void removeWhiteSpace(std::string& line)
    {
        line = mili::trim(line);
    }

    static LineType extractLineType(std::string& line)
    {
        LineType result;

        removeComent(line);
        removeWhiteSpace(line);

        if (line[0] == '>')
            result = lineaDescriptiva;
        else if (line[0] == '\n')
            result = lineaVacia;
        else
            result = lineaSecuencia;

        return result;
    }

public:
    FastaParser(const std::string& file_name) :
        file_name(file_name)
    {}
    ~FastaParser()
    {}
    void load_sequence(std::string& title, Sequence& seq);
    void save_sequence(const std::string& title, const Sequence& seq);
};

template<class Sequence>
void FastaParser<Sequence>::load_sequence(std::string& title, Sequence& seq)
{
    std::ifstream is(file_name.c_str());
    std::string str;
    LineType lt;
    FastaMachine fsm;

    while (std::getline(is, str))
    {
        lt = extractLineType(str);
        if (lt == lineaSecuencia)
        {
            for (size_t i = 0; i < str.size(); i++)
                fsm.procesarLineaSecuencia(str[i]);
        }
        else if (lt == lineaDescriptiva)
        {
            for (size_t i = 0; i < str.size(); i++)
                fsm.procesarLineaDescriptiva(str[i]);
        }
    }

    is.close();
    seq = fsm.getSequence();
    title = fsm.getTitle();
}

template<class Sequence>
void FastaParser<Sequence>::save_sequence(const std::string& title, const Sequence& seq)
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
