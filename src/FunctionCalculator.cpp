#include "FunctionCalculator.h"
#include "SquareMatrix.h"
#include "Add.h"
#include "Sub.h"
#include "Comp.h"
#include "Identity.h"
#include "Transpose.h"
#include "Scalar.h"

#include <iostream>
#include <algorithm>
#include <sstream>


FunctionCalculator::FunctionCalculator(std::istream& istr, std::ostream& ostr)
    : m_actions(createActions()), m_operations(createOperations()), m_istr(istr), m_ostr(ostr)
{
    
}


void FunctionCalculator::run(std::istream& istr)
{
    try
    {
        istr.exceptions(std::ios::failbit | std::ios::badbit);
        m_ostr << '\n';
        printOperations();
        m_ostr << "Enter command ('help' for the list of available commands): ";
    	std::getline(istr, m_inputLine);
        auto iss =  std::istringstream(m_inputLine);
        iss.exceptions(std::ios::failbit | std::ios::badbit);
        const auto action = readAction(iss);
        runAction(action,iss);
        }
		catch (const std::exception& e)
		{
			m_ostr << "Error: " << e.what() << '\n';

            m_istr.clear();
            //m_istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		catch (...)
		{
			m_ostr << "Unknown error occurred\n";
            m_istr.clear();
            //m_istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
}
//=================================
void FunctionCalculator::run()
{
    do
    {
        run(std::cin);
    } while (m_running);
}
//=================================

void FunctionCalculator::eval(std::istringstream& iss)
{
    if (auto index = readOperationIndex(iss); index)
    {
        const auto& operation = m_operations[*index];
		int inputCount = operation->inputCount();
        int size = 0;
        iss >> size;//לבדוק שהגודל לא גדול מ5

        if (size > 5) {
            throw std::invalid_argument("Matrix size cannot be greater than 5");
        }
		if (size <= 0) {
			throw std::invalid_argument("Matrix size must be positive");
		}

		auto matrixVec = std::vector<Operation::T>();
        if (inputCount > 1)
            m_ostr << "\nPlease enter " << inputCount << " matrices:\n";

		for (int i = 0; i < inputCount; ++i)
		{
            auto input = Operation::T(size);
            m_ostr << "\nEnter a " << size << "x" << size << " matrix:\n";
            m_istr >> input;
			matrixVec.push_back(input);

		}
        m_ostr << "\n";
        operation->print(m_ostr, matrixVec);
        m_ostr << " = \n" << operation->compute(matrixVec);
    }
}


void FunctionCalculator::del(std::istringstream& iss)
{
    if (auto i = readOperationIndex(iss); i)
    {
        m_operations.erase(m_operations.begin() + *i);
    }
}


void FunctionCalculator::help()
{
    m_ostr << "The available commands are:\n";
    for (const auto& action : m_actions)
    {
        m_ostr << "* " << action.command << action.description << '\n';
    }
    m_ostr << '\n';
}


void FunctionCalculator::exit()
{
    m_ostr << "Goodbye!\n";
    m_running = false;
}
//=================================
void FunctionCalculator::read(std::istringstream& iss)
{
    std::ifstream file;
    std::string filename;
    std::cout << "Enter file name: ";
	iss >> filename;
    file.open(filename);
	if (!file)
	{
		throw std::invalid_argument("File not found");
	}
    do
    {
	run(file);
    } while (m_running && file.eof());
}
//============================

void FunctionCalculator::printOperations() const
{
    m_ostr << "List of available matrix operations:\n";
    for (decltype(m_operations.size()) i = 0; i < m_operations.size(); ++i)
    {
        m_ostr << i << ". ";
        m_operations[i]->print(m_ostr,true);
        m_ostr << '\n';
    }
    m_ostr << '\n';
}


std::optional<int> FunctionCalculator::readOperationIndex(std::istringstream& iss) const
{
    int i = 0;
    iss >> i;
    if (i >= static_cast<int>(m_operations.size()) || i < 0)
    {
        throw std::invalid_argument("Operation #" + std::to_string(i) + " doesn't exist");
        return {};
    }
    return i;
}


FunctionCalculator::Action FunctionCalculator::readAction(std::istringstream& iss) const
{
    auto action = std::string();
    iss >> action;

    const auto i = std::ranges::find(m_actions, action, &ActionDetails::command);
    if (i != m_actions.end())
    {
        return i->action;
    }

    return Action::Invalid;
}


void FunctionCalculator::runAction(Action action, std::istringstream& iss)
{
    switch (action)
    {
        default:
            m_ostr << "Unknown enum entry used!\n";
            break;

        case Action::Invalid:

            throw std::invalid_argument("Command not found");
            break;

        case Action::Eval:         eval(iss);                     break;
        case Action::Add:          binaryFunc<Add>(iss);          break;
        case Action::Sub:          binaryFunc<Sub>(iss);          break;
        case Action::Comp:         binaryFunc<Comp>(iss);         break;
        case Action::Del:          del(iss);                      break;
        case Action::Help:         help();                     break;
        case Action::Exit:         exit();                     break;
		case Action::Read:         read(iss);                     break;
        //case Action::Iden:          unaryFunc<Identity>();      break;
        //case Action::Tran:          unaryFunc<Transpose>();      break;
        case Action::Scal:          unaryWithIntFunc<Scalar>(iss);      break;
    }
}


FunctionCalculator::ActionMap FunctionCalculator::createActions() const
{
    return ActionMap
    {
        {
            "eval",
            "(uate) num n - compute the result of function #num on an n׳n matrix "
			"(that will be prompted)",
            Action::Eval
        },
        {
            "scal",
            "(ar) val - creates an operation that multiplies the "
			"given matrix by scalar val",
            Action::Scal
        },
        {
            "add",
            " num1 num2 - creates an operation that is the addition of the result of operation #num1 "
			"and the result of operation #num2",
            Action::Add
        },
         {
            "sub",
            " num1 num2 - creates an operation that is the subtraction of the result of operation #num1 "
			"and the result of operation #num2",
            Action::Sub
        },
        {
            "comp",
            "(osite) num1 num2 - creates an operation that is the composition of operation #num1 "
			"and operation #num2",
            Action::Comp
        },
        {
            "del",
            "(ete) num - delete operation #num from the operation list",
            Action::Del
        },
        {
            "help",
            " - print this command list",
            Action::Help
        },
        {
            "exit",
            " - exit the program",
            Action::Exit
        },
		{
			"read",
			" - read from file",
			Action::Read
		}
    };
}


FunctionCalculator::OperationList FunctionCalculator::createOperations() const
{
    return OperationList
    {
        std::make_shared<Identity>(),
        std::make_shared<Transpose>(),
    };
}
