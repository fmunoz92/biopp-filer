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

template<class Sequence>
class FastaParser
{
private:

    static void removeComment(std::string& line)
    {
        const std::string::size_type commentPosistion = line.find_first_of(";");
        if (commentPosistion != std::string::npos)
            line = line.substr(0, commentPosistion);
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
    FastaMachine fsm;
public:

    FastaParser(const std::string& file_name)
        : is(file_name.c_str())
    {
        if (!is.is_open())
            throw FileNotFound(file_name);
    }

    bool getNextSequence(std::string& description, Sequence& sequence);
    bool getNextSequence(std::string& description, std::string& sequence);
};

template<class Sequence>
void FastaParser<Sequence>::stimulateFastaMachine()
{
    std::string line;

    if (std::getline(is, line))
    {
        removeComment(line);
        removeWhiteSpace(line);

        if (line.empty())
        {
            fsm.lineEmpty(line);
        }
        else if (line[0] == '>')
        {
            removeFirstChar(line);
            fsm.lineDescription(line);
        }
        else
            fsm.lineSequence(line);
    }
    else
        fsm.eof();
}

template<class Sequence>
bool FastaParser<Sequence>::getNextSequence(std::string& description, Sequence& sequence)
{
    std::string sequenceString;
    bool result = getNextSequence(description, sequenceString);

    sequence = sequenceString;

    return result;
}

template<class Sequence>
bool FastaParser<Sequence>::getNextSequence(std::string& description, std::string& sequence)
{
    description.clear();
    sequence.clear();
    fsm.setCurrentSequence(sequence, description);

    while (fsm.keepRunning())
        stimulateFastaMachine();

    fsm.reset();

    return fsm.isValidSequence();
}

}
#endif
