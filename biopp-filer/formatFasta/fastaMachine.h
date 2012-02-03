/*
fastaMachine.h: load and save sequences(NucSequence, PseudonucSequence, and AminoSequence)
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

#ifndef FASTA_MACHINE_H
#define FASTA_MACHINE_H

#include <string>

namespace bioppFiler
{

class FastaMachine
{
public:

    typedef std::string LineType;
    typedef std::string Sequence;

    inline FastaMachine();
    inline ~FastaMachine();

    inline void setCurrentSequence(Sequence& seq, LineType& des);
    inline bool isValidSequence() const;
    inline bool keepRunning() const;

    /***************Stimulus**************/
    inline void lineDescription(const LineType& line);
    inline void lineSequence(const LineType& line);
    inline void lineEmpty(const LineType& line);
    inline void eof();

private:

    class State; //abstract interface
    class WaitingForDescription;
    class WaitingForSequence;
    class ReadingSequence;
    class EndOfFile;

    /*
     * return sequence and description to currents
     */
    inline void yield();

    /*
     * reset flags for keepRunning(Sequence*, LineType*)
     */
    inline void resetFlags();

    const State* const waitingForDescription;
    const State* const waitingForSequence;
    const State* const readingSequence;
    const State* const endOfFile;
    const State*       current;

    Sequence* currentSequence;
    LineType* currentDescription;

    Sequence sequence;
    LineType description;

    bool running;
};
}

#define FASTA_MACHINE_INLINE_H
#include "fastaMachine_inline.h"
#undef FASTA_MACHINE_INLINE_H
#endif
