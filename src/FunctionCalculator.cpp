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



FunctionCalculator::FunctionCalculator( std::ostream& ostr)
    : m_actions(createActions()), m_operations(createOperations()), m_ostr(ostr)
{
}


void FunctionCalculator::run(std::istream& istr)
{
        std::string inputLine;
    try
    {
        istr.exceptions(std::ios::failbit | std::ios::badbit);

        m_ostr << '\n';
        printOperations();
        m_ostr <<"The maximum rebellious operators are: " << m_maxOperations <<"\n\n";

        m_ostr << "Enter command ('help' for the list of available commands): ";

        

        inputStringStream iss(istr);

        const auto action = readAction(iss);
        runAction(action, iss, istr);
    }
    catch (const std::exception& e)
    {
        if (m_isFromFile)
        {
			istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw std::invalid_argument(inputStringStream::getLine() + " : Error: " + e.what());
        }

        m_ostr << "Error: " << e.what() << '\n';

    }

}

//=================================
void FunctionCalculator::run()
{
	resize(std::cin);
    do
    {
            run(std::cin);

    } while (m_running);
}
//=================================

void FunctionCalculator::eval(inputStringStream& iss, std::istream& istr)
{
    if (auto index = readOperationIndex(iss); index)
    {
        const auto& operation = m_operations[*index];
		int inputCount = operation->inputCount();
        int size = iss.getInt("Matrix size must be between 1 and 5",1,5);
        iss.checkEndOfInput();

		auto matrixVec = std::vector<Operation::T>();
        if (inputCount > 1)
            m_ostr << "\nPlease enter " << inputCount << " matrices:\n";

    for (int i = 0; i < inputCount; ++i)
    {
        auto input = Operation::T(size);
        m_ostr << "\nEnter a " << size << "x" << size << " matrix:\n";
        istr >> input;
        matrixVec.push_back(input);
    }

        m_ostr << "\n";
        operation->print(m_ostr, matrixVec);
        m_ostr << " = \n" << operation->compute(matrixVec);
    }
}


void FunctionCalculator::del(inputStringStream& iss)
{
    if (auto i = readOperationIndex(iss); i)
    {
        iss.checkEndOfInput();
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
void FunctionCalculator::read(inputStringStream& iss)
{
    
    std::ifstream file;
    std::string filename = iss.getString();
    iss.checkEndOfInput();

    file.open(filename);
	if (!file)
	{
		throw std::invalid_argument("File not found");
	}
    m_isFromFile = true;
    do
    {
        try
        {
            run(file);
            m_isFromFile = true;
        }

        catch(const std::exception& e)
        {
            m_ostr << "\nError in: " << e.what() << "\n"
                <<"Continue reading from the file? (Yes = y,No = Any other buttou): ";
        
            m_isFromFile = false;
            char YesNo;
            inputStringStream iss(std::cin);
            YesNo = iss.getChar();

            if (YesNo == 'y' || YesNo == 'Y')
                m_isFromFile = true;
        }

    } while (m_running && !file.eof() && m_isFromFile);
    m_isFromFile = false;
}
//============================
void FunctionCalculator::resize(std::istream& istr)
{
    int size = 0;
    do
    {
    m_ostr << "\nEnter the number of authorized operators: ";
        try
        {
			inputStringStream iss(istr);
            size = iss.getInt("Max operations cannot be greater than 100 or smaller than 2.", 2, 100);
           	iss.checkEndOfInput();
         
			if (size < m_operations.size())
			{
				m_ostr << "The number of operations is greater than the maximum allowed. To cancel the operation press 1 To delete the unnecessary functions press 2 .\n";
                inputStringStream iss(istr);
				int choice = iss.getInt("The number of operations is greater than the maximum allowed.",1,2);
				iss.checkEndOfInput();
				if (choice == 1)
				{
					m_ostr << "Canceled!.\n";
					break;
				}
				else if (choice == 2)
				{
                    m_operations.erase(m_operations.begin() + size, m_operations.end());

					m_ostr << "The unnecessary functions have been deleted!.\n\n";
				}
			}
            m_maxOperations = size;
        }
        catch (const std::exception& e)
        {
            m_ostr << "Error: " << e.what() << '\n';
        }

        istr.clear();
    }
	while (size != m_maxOperations || size < m_operations.size());
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


std::optional<int> FunctionCalculator::readOperationIndex(inputStringStream& iss) const
{
    int i = iss.getInt("Operation doesn't exist",0, static_cast<int>(m_operations.size())-1);
    return i;
}


FunctionCalculator::Action FunctionCalculator::readAction(inputStringStream& iss) const
{
    auto action = std::string();
    action = iss.getString();

    const auto i = std::ranges::find(m_actions, action, &ActionDetails::command);
    if (i != m_actions.end())
    {
        return i->action;
    }

    return Action::Invalid;
}


void FunctionCalculator::runAction(Action action, inputStringStream& iss, std::istream& istr)
{
    switch (action)
    {
        default:
            m_ostr << "Unknown enum entry used!\n";
            break;

        case Action::Invalid:

            throw std::invalid_argument("Command not found");
            break;

        case Action::Eval:         eval(iss,istr);             break;
        case Action::Add:          checkMaxOperations(); binaryFunc<Add>(iss);       break;
        case Action::Sub:          checkMaxOperations(); binaryFunc<Sub>(iss);       break;
        case Action::Comp:         checkMaxOperations(); binaryFunc<Comp>(iss);      break;
        case Action::Del:          del(iss);                   break;
        case Action::Help:         iss.checkEndOfInput();help();                     break;
        case Action::Exit:         iss.checkEndOfInput();exit();                     break;
		case Action::Read:         read(iss);                  break;
        //case Action::Iden:          unaryFunc<Identity>();      break;
        //case Action::Tran:          unaryFunc<Transpose>();      break;
        case Action::Scal:         checkMaxOperations(); unaryWithIntFunc<Scalar>(iss); break;
        case Action::Resize:       iss.checkEndOfInput();resize(istr);                   break;
    }
}
//=================================
void FunctionCalculator::checkMaxOperations() const
{
	if (m_operations.size() >= m_maxOperations)
	{
		throw std::invalid_argument("The number of operations exceeds the maximum allowed");
	}
}
//=================================
FunctionCalculator::ActionMap FunctionCalculator::createActions() const
{
    return ActionMap
    {
        {
            "eval",
            "(uate) num n - compute the result of function #num on an n×n matrix "
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
        },
        {
            "resize",
            " - resize the max options",
            Action::Resize
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
