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
#include "fastaMachine.h"

namespace bioppFiler
{

template<class Sequence>
class FastaParser
{
private:

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

    void stimulateFastaMachine();

    std::ifstream is;
    FastaMachine<Sequence> fsm;
public:

    FastaParser(const std::string& file_name)
        : is(file_name.c_str()),
          fsm()
    {
        if (!is.is_open())
            throw FileNotFound(file_name);
    }

    ~FastaParser()
    {
        is.close();
    }

    bool getNextSequence(std::string& description, Sequence& sequence);
};

template<class Sequence>
void FastaParser<Sequence>::stimulateFastaMachine()
{
    static std::string line;
    if (std::getline(is, line))
    {
        removeComment(line);
        removeWhiteSpace(line);

        if (line[0] == '>')
        {
            removeFirstChar(line);
            fsm.lineDescription(line);
        }
        else if (line.size() == 0)
            fsm.lineEmpty(line);
        else
            fsm.lineSequence(line);
    }
    else
        fsm.eof();
}

template<class Sequence>
bool FastaParser<Sequence>::getNextSequence(std::string& description, Sequence& sequence)
{
    description.clear();
    sequence.clear();
    fsm.setCurrentSequence(&sequence, &description);

    while (fsm.keepRunning())
        stimulateFastaMachine();

    fsm.reset();

    return sequence.length() != 0;
}

}
#endif