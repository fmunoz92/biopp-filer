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

#ifndef FSM_H
#define FSM_H

#include <string>

namespace bioppFiler
{

typedef std::string DataType;

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
        virtual const State* lineDescription(const DataType& line) const = 0;
        virtual const State* lineSequence(const DataType& line) const = 0;
        virtual const State* lineEmpty() const = 0;
        virtual const State* eof() const = 0;
    };

    class WaitingForSequence : public State
    {
    public:
        WaitingForSequence(FastaMachine* fm)
            : State(fm)
        {}
        const State* lineDescription(const DataType& line) const;
        const State* lineSequence(const DataType& line) const;
        const State* lineEmpty() const;
        const State* eof() const;
    };

    class ReadingSequence : public State
    {
    public:
        ReadingSequence(FastaMachine* fm)
            : State(fm)
        {}
        const State* lineDescription(const DataType& line) const;
        const State* lineSequence(const DataType& line) const;
        const State* lineEmpty() const;
        const State* eof() const;
    };

    class WaitingForDescription : public State
    {
    public:
        WaitingForDescription(FastaMachine* fm)
            : State(fm)
        {}
        const State* lineDescription(const DataType& line) const;
        const State* lineSequence(const DataType& line) const;
        const State* lineEmpty() const;
        const State* eof() const;
    };

    class Exit : public State
    {
    public:
        Exit(FastaMachine* fm)
            : State(fm)
        {}
        const State* lineDescription(const DataType& line) const;
        const State* lineSequence(const DataType& line) const;
        const State* lineEmpty() const;
        const State* eof() const;
    };

    const State* const waitingForSequence;
    const State* const waitingForDescription;
    const State* const readingSequence;
    const State* const exit;
    const State* current;

    bool previousDescription;
    DataType linePreviousDescription;

    void saveStimulusDescription(const DataType& line)
    {
        previousDescription = true;
        linePreviousDescription = line;
    }

    void runStackStimulus()
    {
        if (previousDescription)
        {
            current = current->lineDescription(linePreviousDescription);
            previousDescription = false;
        }
    }

    Sequence sequence;
    DataType description;

public:

    FastaMachine()
        : waitingForSequence(new WaitingForSequence(this)),
          waitingForDescription(new WaitingForDescription(this)),
          readingSequence(new ReadingSequence(this)),
          exit(new Exit(this)),
          current(waitingForDescription),
          previousDescription(false),
          linePreviousDescription(),
          sequence(),
          description()
    {}

    ~FastaMachine()
    {
        delete waitingForSequence;
        delete waitingForDescription;
        delete readingSequence;
        delete exit;
    }

    void lineDescription(const DataType& line);
    void lineSequence(const DataType& line);
    void lineEmpty(const DataType& line);
    void eof();

    bool isRunning() const
    {
        return current != exit;
    }

    void getSequence(Sequence& seq, DataType& des) const
    {
        seq = sequence;
        des = description;
    }
};

template<class Sequence>
void FastaMachine<Sequence>::lineDescription(const DataType& line)
{
    runStackStimulus();
    current = current->lineDescription(line);
}

template<class Sequence>
void FastaMachine<Sequence>::lineSequence(const DataType& line)
{
    runStackStimulus();
    current = current->lineSequence(line);
}

template<class Sequence>
void FastaMachine<Sequence>::lineEmpty(const DataType&)
{
    runStackStimulus();
    current = current->lineEmpty();
}

template<class Sequence>
void FastaMachine<Sequence>::eof()
{
    runStackStimulus();
    current = current->eof();
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForSequence::lineDescription(const DataType&) const
{
    throw FileError(string("WaitingForSequence, Expected lineSequence"));
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForSequence::lineSequence(const DataType& line) const
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
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::lineDescription(const DataType& line) const
{
    this->fsm->saveStimulusDescription(line);

    return this->fsm->exit;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::lineEmpty() const
{
    return this->fsm->exit;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::lineSequence(const DataType& line) const
{
    DataType tmp = this->fsm->sequence.getString();
    tmp += line;
    this->fsm->sequence = tmp;

    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::eof() const
{
    return this->fsm->exit;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForDescription::lineDescription(const DataType& line) const
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
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForDescription::lineSequence(const DataType& line) const
{
    this->fsm->sequence = line;
    this->fsm->description.clear();

    return this->fsm->readingSequence;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForDescription::eof() const
{
    return this->fsm->exit;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Exit::lineDescription(const DataType& line) const
{
    this->fsm->description = line;

    return this->fsm->waitingForSequence;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Exit::lineEmpty() const
{
    this->fsm->description.clear();
    this->fsm->sequence.clear();

    return this->fsm->waitingForSequence;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Exit::lineSequence(const DataType& line) const
{
    this->fsm->sequence = line;
    this->fsm->description.clear();

    return this->fsm->readingSequence;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Exit::eof() const
{
    this->fsm->description.clear();
    this->fsm->sequence.clear();

    return this;
}

}
#endif
