/*
fastaParser_inline.h: load and save sequences(NucSequence, PseudonucSequence, and AminoSequence)
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

#ifndef FASTA_PARSER_INLINE_H
#error Internal header file, DO NOT include this.
#endif

namespace bioppFiler
{

template<class SequenceType>
inline FastaParser<SequenceType>::FastaParser(const std::string& file_name)
    : is(file_name.c_str())
{
    if (!is.is_open())
        throw FileNotFound(file_name);
}

template<class SequenceType>
inline void FastaParser<SequenceType>::removeComment(std::string& line)
{
    const std::string::size_type commentPosistion = line.find_first_of(";");
    if (commentPosistion != std::string::npos)
        line = line.substr(0, commentPosistion);
}

template<class SequenceType>
inline void FastaParser<SequenceType>::removeFirstChar(std::string& line)
{
    line = line.substr(1, line.size());
}

template<class SequenceType>
inline void FastaParser<SequenceType>::removeWhiteSpace(std::string& line)
{
    line = mili::trim(line);
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
}

template<class SequenceType>
inline void FastaParser<SequenceType>::stimulateFastaMachine()
{
    std::string line;

    if (std::getline(is, line))
    {
        removeComment(line);
        removeWhiteSpace(line);

        if (line.empty())
        {
            fsm.lineEmpty();
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

template<class SequenceType>
inline bool FastaParser<SequenceType>::getNextSequence(std::string& description, SequenceType& sequence)
{
    std::string sequenceString;//for type conversion
    const bool result = getNextSequence(description, sequenceString);

    sequence = SequenceType(sequenceString);

    return result;
}

template<class SequenceType>
inline bool FastaParser<SequenceType>::getNextSequence(std::string& description, std::string& sequence)
{
    description.clear();
    sequence.clear();
    fsm.setCurrentSequence(sequence, description);

    do
        stimulateFastaMachine();
    while (fsm.keepRunning());

    return fsm.isValidSequence();
}

}
