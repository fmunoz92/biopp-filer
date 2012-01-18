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
#include "bioppFilerException.h"

typedef std::string DataType;


DEFINE_SPECIFIC_EXCEPTION(FileError, BioppFilerException);

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
        virtual const State* procesarLineaDescriptiva(DataType c) const = 0;
        virtual const State* procesarLineaSecuencia(DataType c) const = 0;
        virtual const State* procesarLineaVacia() const = 0;
    };

    class WaitingForSequence : public State
    {
    public:
        WaitingForSequence(FastaMachine* fm)
            : State(fm)
        {}
        const State* procesarLineaDescriptiva(DataType c) const;
        const State* procesarLineaSecuencia(DataType c) const;
        const State* procesarLineaVacia() const;
    };

    class ReadingSequence : public State
    {
    public:
        ReadingSequence(FastaMachine* fm)
            : State(fm)
        {}
        const State* procesarLineaDescriptiva(DataType c) const;
        const State* procesarLineaSecuencia(DataType c) const;
        const State* procesarLineaVacia() const;
    };

    class WaitingDescription : public State
    {
    public:
        WaitingDescription(FastaMachine* fm)
            : State(fm)
        {}
        const State* procesarLineaDescriptiva(DataType c) const;
        const State* procesarLineaSecuencia(DataType c) const;
        const State* procesarLineaVacia() const;
    };

    class Error : public State
    {
    public:
        Error(FastaMachine* fm)
            : State(fm)
        {}
        const State* procesarLineaDescriptiva(DataType c) const;
        const State* procesarLineaSecuencia(DataType c) const;
        const State* procesarLineaVacia() const;
    };

    const State* const waitingForSequence;
    const State* const waitingDescription;
    const State* const readingSequence;
    const State* const error;
    const State* current;
    bool descripcionYaLeida;
    Sequence& secuencia;
    DataType& descripcion;

public:
    FastaMachine(Sequence& seq, DataType& des)
        : waitingForSequence(new WaitingForSequence(this)),
          waitingDescription(new WaitingDescription(this)),
          readingSequence   (new ReadingSequence   (this)),
          error             (new Error             (this)),
          current(waitingForSequence),
          descripcionYaLeida(false),
          secuencia(seq),
          descripcion(des)
    {}

    void procesarLineaDescriptiva(DataType c);
    void procesarLineaSecuencia(DataType c);
    void procesarLineaVacia(DataType c);
};

template<class Sequence>
void FastaMachine<Sequence>::procesarLineaDescriptiva(DataType c)
{
    current = current->procesarLineaDescriptiva(c);
}

template<class Sequence>
void FastaMachine<Sequence>::procesarLineaSecuencia(DataType c)
{
    current = current->procesarLineaSecuencia(c);
}

template<class Sequence>
void FastaMachine<Sequence>::procesarLineaVacia(DataType c)
{
    if (c.size() != 0)
        throw FileError(string("linea no vacia"));
    current = current->procesarLineaVacia();
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForSequence::procesarLineaDescriptiva(DataType c) const
{
    return this->fsm->waitingDescription->procesarLineaDescriptiva(c); //o this->fsm->stack_state.push(this->fsm->waitingDescription);  ?
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForSequence::procesarLineaSecuencia(DataType c) const
{
    return this->fsm->readingSequence->procesarLineaSecuencia(c);//o this->fsm->stack_state.push(this->fsm->readingSequence); ?
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingForSequence::procesarLineaVacia() const
{
    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::procesarLineaDescriptiva(DataType c) const
{
    return this->fsm->error->procesarLineaDescriptiva(c);
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::procesarLineaVacia() const
{
    return this->fsm->waitingForSequence;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::ReadingSequence::procesarLineaSecuencia(DataType c) const
{
    DataType tmp = this->fsm->secuencia.getString();
    tmp += c;
    this->fsm->secuencia = tmp;

    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingDescription::procesarLineaDescriptiva(DataType c) const
{
    const FastaMachine::State* result = this->fsm->error;

    if (!this->fsm->descripcionYaLeida)
    {
        this->fsm->descripcion = c;
        result =  this->fsm->waitingForSequence;
        this->fsm->descripcionYaLeida = true;
    }

    return result;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingDescription::procesarLineaVacia() const
{
    return this->fsm->error;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::WaitingDescription::procesarLineaSecuencia(DataType c) const
{
    return this->fsm->readingSequence->procesarLineaSecuencia(c);//othis->fsm->stack_state.push(this->fsm->readingSequence);
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Error::procesarLineaDescriptiva(DataType c) const
{
    throw FileError(string("linea no procesada: ") + c);
    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Error::procesarLineaVacia() const
{
    throw FileError(string("linea vacia no procesada"));
    return this;
}

template<class Sequence>
inline const typename FastaMachine<Sequence>::State* FastaMachine<Sequence>::Error::procesarLineaSecuencia(DataType c) const
{
    throw FileError(string("linea no procesada: ") + c);
    return this;
}

#endif
