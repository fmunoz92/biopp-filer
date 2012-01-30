/*
fsm.h: load and save sequences(NucSequence, PseudonucSequence, and AminoSequence)
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

template<class Sequence>
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
        *current_description = description;
        *current_sequence    = sequence;
        running              = false;
    }

    const State* const waitingForDescription;
    const State* const waitingForSequence;
    const State* const readingSequence;
    const State* const endOfFile;
    const State*       current;

    Sequence* current_sequence;
    LineType* current_description;

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
          current_sequence(NULL),
          current_description(NULL),
          sequence(),
          description(),
          running(true)
    {}

    ~FastaMachine()
    {
        delete waitingForSequence;
        delete waitingForDescription;
        delete readingSequence;
        delete endOfFile;
    }

    void setCurrentSequence(Sequence* seq, LineType* des)
    {
        current_sequence    = seq;
        current_description = des;
    }

    bool keepRunning() const
    {
        return running && (current != endOfFile);
    }

    /*
     * reset flags for keepRunning(Sequence*, LineType*)
     */
    void reset()
    {
        running = true;
    }

    /***************Stimulus**************/
    void lineDescription(const LineType& line);
    void lineSequence(const LineType& line);
    void lineEmpty(const LineType& line);
    void eof();
};

template<class Sequence>
void FastaMachine<Sequence>::lineDescription(const LineType& line)
{
    current = current->lineDescription(line);
}

template<class Sequence>
void FastaMachine<Sequence>::lineSequence(const LineType& line)
{
    current = current->lineSequence(line);
}

template<class Sequence>
void FastaMachine<Sequence>::lineEmpty(const LineType&)
{
    current = current->lineEmpty();
}

template<class Sequence>
void FastaMachine<Sequence>::eof()
{
    current = current->eof();
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForDescription::lineDescription(const LineType& line) const
{
    this->fsm->description = line;

    return this->fsm->waitingForSequence;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForDescription::lineEmpty() const
{
    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForDescription::lineSequence(const LineType& line) const
{
    this->fsm->sequence = line;

    return this->fsm->readingSequence;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForDescription::eof() const
{
    this->fsm->yield();

    return this->fsm->endOfFile;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForSequence::lineDescription(const LineType&) const
{
    throw FileError(string("WaitingForSequence, Expected lineSequence"));
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForSequence::lineSequence(const LineType& line) const
{
    this->fsm->sequence = line;

    return this->fsm->readingSequence;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForSequence::lineEmpty() const
{
    throw FileError(string("WaitingForSequence, Expected lineSequence"));
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForSequence::eof() const
{
    throw FileError(string("WaitingForSequence, Expected lineSequence"));
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::lineDescription(const LineType& line) const
{
    this->fsm->yield();
    this->fsm->description = line;

    return this->fsm->waitingForSequence;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::lineEmpty() const
{
    this->fsm->yield();
    this->fsm->description.clear();
    this->fsm->sequence.clear();

    return this->fsm->waitingForDescription;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::lineSequence(const LineType& line) const
{
    LineType tmp = this->fsm->sequence.getString();
    tmp += line;
    this->fsm->sequence = tmp;

    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::eof() const
{
    this->fsm->yield();

    return this->fsm->endOfFile;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::EndOfFile::lineDescription(const LineType&) const
{
    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::EndOfFile::lineEmpty() const
{
    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::EndOfFile::lineSequence(const LineType&) const
{
    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::EndOfFile::eof() const
{
    return this;
}

}
#endif
