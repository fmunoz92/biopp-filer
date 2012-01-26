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
        virtual const State* reset() const;//predefined
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
    };

    class Error : public State
    {
    public:
        Error(FastaMachine* fm)
            : State(fm)
        {}
        const State* lineDescription(const DataType& line) const;
        const State* lineSequence(const DataType& line) const;
        const State* lineEmpty() const;
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
    };

    const State* const waitingForSequence;
    const State* const waitingForDescription;
    const State* const readingSequence;
    const State* const error;
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
          error(new Error(this)),
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
        delete error;
        delete exit;
    }

    void lineDescription(const DataType& line);
    void lineSequence(const DataType& line);
    void lineEmpty(const DataType& line);
    void reset();

    bool isRunning() const
    {
        return (current != exit) && (current != error);//is valid?
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
void FastaMachine<Sequence>::lineEmpty(const DataType& line)
{
    runStackStimulus();
    if (line.size() != 0)
        throw FileError(std::string("non-empty line"));
    current = current->lineEmpty();
}

template<class Sequence>
void FastaMachine<Sequence>::reset()
{
    current = current->reset();
    runStackStimulus();
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::State::reset() const
{
    this->fsm->sequence.clear();
    this->fsm->description.clear();

    return this->fsm->waitingForDescription;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForSequence::lineDescription(const DataType& line) const
{
    throw FileError(string("Expected lineSequence, ") + line);
    return this->fsm->error;
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
    throw FileError(string("Expected lineSequence"));
    return this->fsm->error;
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

    return this->fsm->readingSequence;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Error::lineDescription(const DataType& line) const
{
    throw FileError(string("State Error ") + line);
    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Error::lineEmpty() const
{
    throw FileError(string("State Error, empty line"));
    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Error::lineSequence(const DataType& line) const
{
    throw FileError(string("State Error ") + line);
    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Exit::lineDescription(const DataType& line) const
{
    throw FileError(string("Expected reset, " + line));

    return this->fsm->error;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Exit::lineEmpty() const
{
    throw FileError(string("Expected reset"));

    return this->fsm->error;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Exit::lineSequence(const DataType& line) const
{
    throw FileError(string("Expected reset, " + line));

    return this->fsm->error;
}

}
#endif
