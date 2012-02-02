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

    FastaMachine()
        : waitingForDescription(new WaitingForDescription(this)),
          waitingForSequence(new WaitingForSequence(this)),
          readingSequence(new ReadingSequence(this)),
          endOfFile(new EndOfFile(this)),
          current(waitingForDescription),
          running(true)
    {}

    ~FastaMachine()
    {
        delete waitingForSequence;
        delete waitingForDescription;
        delete readingSequence;
        delete endOfFile;
    }

    void setCurrentSequence(Sequence& seq, LineType& des);
    bool isValidSequence() const;
    bool keepRunning() const;

    /***************Stimulus**************/
    void lineDescription(const LineType& line);
    void lineSequence(const LineType& line);
    void lineEmpty(const LineType& line);
    void eof();

private:

    class State //abstract interface
    {
    protected:
        FastaMachine* const fsm;
    public:
        State(FastaMachine* fm)
            : fsm(fm)
        {}
        virtual ~State()
        {}
        virtual const State* lineDescription(const LineType& line) const = 0;
        virtual const State* lineSequence(const LineType& line) const = 0;
        virtual const State* lineEmpty() const = 0;
        virtual const State* eof() const = 0;
    };

    class WaitingForDescription : public State
    {
    public:
        WaitingForDescription(FastaMachine* fm)
            : State(fm)
        {}
        const State* lineDescription(const LineType& line) const;
        const State* lineSequence(const LineType& line) const;
        const State* lineEmpty() const;
        const State* eof() const;
    };

    class WaitingForSequence : public State
    {
    public:
        WaitingForSequence(FastaMachine* fm)
            : State(fm)
        {}
        const State* lineDescription(const LineType& line) const;
        const State* lineSequence(const LineType& line) const;
        const State* lineEmpty() const;
        const State* eof() const;
    };

    class ReadingSequence : public State
    {
    public:
        ReadingSequence(FastaMachine* fm)
            : State(fm)
        {}
        const State* lineDescription(const LineType& line) const;
        const State* lineSequence(const LineType& line) const;
        const State* lineEmpty() const;
        const State* eof() const;
    };

    class EndOfFile : public State
    {
    public:
        EndOfFile(FastaMachine* fm)
            : State(fm)
        {}
        const State* lineDescription(const LineType& line) const;
        const State* lineSequence(const LineType& line) const;
        const State* lineEmpty() const;
        const State* eof() const;
    };

    /*
     * return sequence and description to currents
     */
    void yield();

    /*
     * reset flags for keepRunning(Sequence*, LineType*)
     */
    void resetFlags();

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
