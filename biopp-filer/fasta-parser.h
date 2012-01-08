//TODO: static LineType extractLineType(std::string& line);
//procesarLineaVacia de todos

#ifndef FASTA_PARSER_H
#define FASTA_PARSER_H

#include <string>
#include <fstream>
#include <stack>
#include <mili/mili.h>

using namespace mili;


template<class Sequence>
class FastaParser
{
private:
    std::string file_name;
    
	enum LineType {lineaSecuencia, lineaDescriptiva, lineaVacia};
	
	static void removeComent(std::string& line)
	{
		size_t comentPosistion = line.find_first_of(";");
		line = line.substr(0, comentPosistion);
	}

	static void removeWhiteSpace(std::string& line)
	{
		line = trim(line);
	}

	static LineType extractLineType(std::string& line)
	{
		LineType result;
		removeComent(line);
		removeWhiteSpace(line);
		if(line[0] == '>')
			result = lineaDescriptiva;
		else if (line[0] == '\n')
			result = lineaVacia;
		else
			result = lineaSecuencia;

		return result;
	}

public:
    FastaParser(const std::string file_name) : 
		file_name(file_name)
    {}
    ~FastaParser()
    {}
    void load_sequence(std::string& title, Sequence& seq);
    //void save_sequence(const std::string& title, const Sequence& seq);
};

class FastaMachine
{
private:
    class State
    {
    protected:
        FastaMachine* fsm;
    public:
        State(FastaMachine* fm) : 
			fsm(fm)
        {}
        virtual ~State()
        {}
        virtual const State* procesarLineaDescriptiva(char c) const = 0;
        virtual const State* procesarLineaSecuencia(char c) const = 0;
        virtual const State* procesarLineaVacia(char c) const = 0;
    };

    class EsperandoContenido : public State
    {
    public:
        EsperandoContenido(FastaMachine* fm) : State(fm)
        {}
        const State* procesarLineaDescriptiva(char c) const;
        const State* procesarLineaSecuencia(char c) const;
        const State* procesarLineaVacia(char c) const;	};

    class LeyendoContenido : public State
    {
    public:
        LeyendoContenido(FastaMachine* fm) : State(fm)
        {}
        const State* procesarLineaDescriptiva(char c) const;
        const State* procesarLineaSecuencia(char c) const;
        const State* procesarLineaVacia(char c) const;
	};

    const State* const esperandoContenido;
    const State* const leyendoContenido;
    const State* current;
    std::stack<const State*> stack_state;
    std::string secuencia;
    std::string descripcion;

public:
    FastaMachine() : 
		esperandoContenido(new EsperandoContenido(this)),
		leyendoContenido(new LeyendoContenido(this)),
        current(esperandoContenido),
        stack_state(),
        secuencia(),
        descripcion()
    {}

	void procesarLineaDescriptiva(char c);
    void procesarLineaSecuencia(char c);
    void procesarLineaVacia(char c);

    std::string getSequence() const { return secuencia;  }
    std::string getTitle()    const { return descripcion;}
};

template<class Sequence>
void FastaParser<Sequence>::load_sequence(std::string& title, Sequence& seq)
{
        std::ifstream is(file_name.c_str());
		std::string str;
		LineType lt;
        FastaMachine fsm;
        while(std::getline(is, str))
        {
			lt = extractLineType(str);
			if (lt == lineaSecuencia)
			{
				for(size_t i = 0; i < str.size(); i++)
					fsm.procesarLineaSecuencia(str[i]);
			}
			else if (lt == lineaDescriptiva)
			{
				for(size_t i = 0; i < str.size(); i++)
					fsm.procesarLineaDescriptiva(str[i]);
			}
			else
			{
				for(size_t i = 0; i < str.size(); i++)
					fsm.procesarLineaVacia(str[i]);
			}
		}

		is.close();
        seq = fsm.getSequence();
        title = fsm.getTitle();
}


void FastaMachine::procesarLineaDescriptiva(char c)
{
    current = current->procesarLineaDescriptiva(c);
	while (!stack_state.empty())
    {
        current = (stack_state.top())->procesarLineaDescriptiva(c);
        stack_state.pop();
    }
}

void FastaMachine::procesarLineaSecuencia(char c)
{
    current = current->procesarLineaSecuencia(c);
	while (!stack_state.empty())
    {
        current = (stack_state.top())->procesarLineaSecuencia(c);
        stack_state.pop();
    }
}

void FastaMachine::procesarLineaVacia(char c)
{
    current = current->procesarLineaSecuencia(c);
    while (!stack_state.empty())
    {
        current = (stack_state.top())->procesarLineaVacia(c);
        stack_state.pop();
    }
}

const FastaMachine::State* FastaMachine::EsperandoContenido::procesarLineaDescriptiva(char c) const
{
    const State* result_state = this;

	if(c != '\n')
    {
        result_state = fsm->leyendoContenido;
        fsm->stack_state.push(fsm->leyendoContenido);
    }

    return result_state;
}

const FastaMachine::State* FastaMachine::EsperandoContenido::procesarLineaSecuencia(char c) const
{
    const State* result_state = this;

	if(c != '\n')
    {
        result_state = fsm->leyendoContenido;
        fsm->stack_state.push(fsm->leyendoContenido);
    }

    return result_state;
}

const FastaMachine::State* FastaMachine::EsperandoContenido::procesarLineaVacia(char c) const
{
    return this;
}

const FastaMachine::State* FastaMachine::LeyendoContenido::procesarLineaDescriptiva(char c) const
{
    const State* result_state = this;

	if(c != '>')
		fsm->descripcion += c;
	if (c == '\n')
        result_state = fsm->esperandoContenido;

    return result_state;
}

const FastaMachine::State* FastaMachine::LeyendoContenido::procesarLineaSecuencia(char c) const
{
    const State* result_state = this;

	if(c != '\n')
        fsm->secuencia += c;
	else
		result_state = fsm->esperandoContenido;

    return result_state;
}

const FastaMachine::State* FastaMachine::LeyendoContenido::procesarLineaVacia(char c) const
{
    return fsm->esperandoContenido;
}
#endif
