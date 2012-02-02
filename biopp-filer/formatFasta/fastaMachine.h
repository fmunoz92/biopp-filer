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

typedef std::string LineType;
typedef std::string Sequence;

class FastaMachine
{
private:
    class State
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

    void yield()
    {
        *currentDescription = description;
        *currentSequence    = sequence;
        running             = false;
    }

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

public:

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

    void setCurrentSequence(Sequence& seq, LineType& des)
    {
        currentSequence    = &seq;
        currentDescription = &des;
    }

    bool isValidSequence()
    {
        return !currentSequence->empty();
    }

    bool keepRunning() const
    {
        return running && (current != endOfFile);
    }

    /***************Stimulus**************/
    void lineDescription(const LineType& line);
    void lineSequence(const LineType& line);
    void lineEmpty(const LineType& line);
    void eof();
};

void FastaMachine::lineDescription(const LineType& line)
{
    running =  true;
    current = current->lineDescription(line);
}

void FastaMachine::lineSequence(const LineType& line)
{
    running =  true;
    current = current->lineSequence(line);
}

void FastaMachine::lineEmpty(const LineType&)
{
    running =  true;
    current = current->lineEmpty();
}

void FastaMachine::eof()
{
    running =  true;
    current = current->eof();
}

inline const FastaMachine::State* FastaMachine::WaitingForDescription::lineDescription(const LineType& line) const
{
    this->fsm->description = line;

    return this->fsm->waitingForSequence;
}

inline const FastaMachine::State* FastaMachine::WaitingForDescription::lineEmpty() const
{
    return this;
}

inline const FastaMachine::State* FastaMachine::WaitingForDescription::lineSequence(const LineType& line) const
{
    this->fsm->sequence = line;

    return this->fsm->readingSequence;
}

inline const FastaMachine::State* FastaMachine::WaitingForDescription::eof() const
{
    this->fsm->yield();

    return this->fsm->endOfFile;
}

inline const FastaMachine::State* FastaMachine::WaitingForSequence::lineDescription(const LineType&) const
{
    throw FileError(string("WaitingForSequence, Expected lineSequence"));
}

inline const FastaMachine::State* FastaMachine::WaitingForSequence::lineSequence(const LineType& line) const
{
    this->fsm->sequence = line;

    return this->fsm->readingSequence;
}

inline const FastaMachine::State* FastaMachine::WaitingForSequence::lineEmpty() const
{
    throw FileError(string("WaitingForSequence, Expected lineSequence"));
}

inline const FastaMachine::State* FastaMachine::WaitingForSequence::eof() const
{
    throw FileError(string("WaitingForSequence, Expected lineSequence"));
}

inline const FastaMachine::State* FastaMachine::ReadingSequence::lineDescription(const LineType& line) const
{
    this->fsm->yield();
    this->fsm->description = line;

    return this->fsm->waitingForSequence;
}

inline const FastaMachine::State* FastaMachine::ReadingSequence::lineEmpty() const
{
    this->fsm->yield();
    this->fsm->description.clear();
    this->fsm->sequence.clear();

    return this->fsm->waitingForDescription;
}

inline const FastaMachine::State* FastaMachine::ReadingSequence::lineSequence(const LineType& line) const
{
    this->fsm->sequence += line;

    return this;
}

inline const FastaMachine::State* FastaMachine::ReadingSequence::eof() const
{
    this->fsm->yield();

    return this->fsm->endOfFile;
}

inline const FastaMachine::State* FastaMachine::EndOfFile::lineDescription(const LineType&) const
{
    return this;
}

inline const FastaMachine::State* FastaMachine::EndOfFile::lineEmpty() const
{
    return this;
}

inline const FastaMachine::State* FastaMachine::EndOfFile::lineSequence(const LineType&) const
{
    return this;
}

inline const FastaMachine::State* FastaMachine::EndOfFile::eof() const
{
    return this;
}

}
#endif
