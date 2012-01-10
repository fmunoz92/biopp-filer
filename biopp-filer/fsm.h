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
#include <stack>

typedef char DataType;

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
    };

    class EsperandoContenido : public State
    {
    public:
        EsperandoContenido(FastaMachine* fm)
            : State(fm)
        {}
        const State* procesarLineaDescriptiva(DataType c) const;
        const State* procesarLineaSecuencia(DataType c) const;
    };

    class LeyendoContenido : public State
    {
    public:
        LeyendoContenido(FastaMachine* fm)
            : State(fm)
        {}
        const State* procesarLineaDescriptiva(DataType c) const;
        const State* procesarLineaSecuencia(DataType c) const;
    };

    const State* const esperandoContenido;
    const State* const leyendoContenido;
    const State* current;
    std::stack<const State*> stack_state;
    std::string secuencia;
    std::string descripcion;

public:
    FastaMachine()
        : esperandoContenido(new EsperandoContenido(this)),
          leyendoContenido(new LeyendoContenido(this)),
          current(esperandoContenido),
          stack_state(),
          secuencia(),
          descripcion()
    {}

    void procesarLineaDescriptiva(DataType c);
    void procesarLineaSecuencia(DataType c);

    std::string getSequence() const
    {
        return secuencia;
    }
    std::string getTitle()    const
    {
        return descripcion;
    }
};

void FastaMachine::procesarLineaDescriptiva(DataType c)
{
    current = current->procesarLineaDescriptiva(c);
    while (!stack_state.empty())
    {
        current = (stack_state.top())->procesarLineaDescriptiva(c);
        stack_state.pop();
    }
}

void FastaMachine::procesarLineaSecuencia(DataType c)
{
    current = current->procesarLineaSecuencia(c);
    while (!stack_state.empty())
    {
        current = (stack_state.top())->procesarLineaSecuencia(c);
        stack_state.pop();
    }
}

const FastaMachine::State* FastaMachine::EsperandoContenido::procesarLineaDescriptiva(DataType c) const
{
    const State* result_state = this;

    if (c != '\n')
    {
        result_state = fsm->leyendoContenido;
        fsm->stack_state.push(fsm->leyendoContenido);
    }

    return result_state;
}

const FastaMachine::State* FastaMachine::EsperandoContenido::procesarLineaSecuencia(DataType c) const
{
    const State* result_state = this;

    if (c != '\n')
    {
        result_state = fsm->leyendoContenido;
        fsm->stack_state.push(fsm->leyendoContenido);
    }

    return result_state;
}

const FastaMachine::State* FastaMachine::LeyendoContenido::procesarLineaDescriptiva(DataType c) const
{
    const State* result_state = this;

    if (c != '>')
        fsm->descripcion += c;
    if (c == '\n')
        result_state = fsm->esperandoContenido;

    return result_state;
}

const FastaMachine::State* FastaMachine::LeyendoContenido::procesarLineaSecuencia(DataType c) const
{
    const State* result_state = this;

    if (c != '\n')
        fsm->secuencia += c;
    else
        result_state = fsm->esperandoContenido;

    return result_state;
}

#endif
