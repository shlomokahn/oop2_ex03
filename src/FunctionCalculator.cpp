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
    try
    {
        istr.exceptions(std::ios::failbit | std::ios::badbit);

        m_ostr << '\n';
        printOperations();
        m_ostr <<"The maximum rebellious operators are: " << m_maxOperations <<"\n\n";

        m_ostr << "Enter command ('help' for the list of available commands): ";

        std::string inputLine;
        std::getline(istr, inputLine);

        auto iss = std::istringstream(inputLine);
        iss.exceptions(std::ios::failbit | std::ios::badbit);

        const auto action = readAction(iss);
        runAction(action, iss, istr);
    }
    catch (const std::exception& e)
    {
        m_ostr << "Error: " << e.what() << '\n';
        istr.clear();
        m_isFromFile = false;
    }
    catch (...)
    {
        m_ostr << "Unknown error occurred\n";
        istr.clear();
        m_isFromFile = false;
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

void FunctionCalculator::eval(std::istringstream& iss, std::istream& istr)
{
    if (auto index = readOperationIndex(iss); index)
    {
        const auto& operation = m_operations[*index];
		int inputCount = operation->inputCount();
        int size = 0;
        iss >> size;
        checkEndOfInput(iss);

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
        istr >> input;
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
        checkEndOfInput(iss);
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
	iss >> filename;
    checkEndOfInput(iss);
    file.open(filename);
	if (!file)
	{
		throw std::invalid_argument("File not found");
	}
    m_isFromFile = true;
    do
    {
	    run(file);
        if(!m_isFromFile)
        {
            m_ostr << "Continue reading from the file? (Yes = y,No = Any other buttou): ";
			char YesNo;
            std::cin >> YesNo;
            if (YesNo == 'y' || YesNo == 'Y')
                m_isFromFile = true;

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

    } while (m_running && !file.eof() && m_isFromFile);
}
//==============================
void FunctionCalculator::checkEndOfInput(std::istringstream& iss)
{
	if (iss.eof())
		return;

    iss >> std::ws;
    if (!iss.eof())
        throw std::invalid_argument("Too many characters");
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
            if (!(istr >> size))
            {
                throw std::invalid_argument("Invalid input. Please enter a valid integer.");
            }
            // לבדוק שאין עוד תוים
            if (size > 100 || size < 2)
            {
                throw std::invalid_argument("Max operations cannot be greater than 100 or smaller than 2.");
            }
			if (size < m_operations.size())
			{
				m_ostr << "The number of operations is greater than the maximum allowed. To cancel the operation press 1 To delete the unnecessary functions press 2 .\n";
				int choice = 0;
				istr >> choice;
				if (choice == 1)
				{
					m_ostr << "Canceled!.\n";
					break;
				}
				else if (choice == 2)
				{
					for (int i = m_operations.size() - 1; i >= size; --i)
					{
						m_operations.erase(m_operations.begin() + i);
					}
					m_ostr << "The unnecessary functions have been deleted!.\n\n";
				}
				else
				{
					throw std::invalid_argument("The number of operations is greater than the maximum allowed.");
				}
			}
            m_maxOperations = size;
        }
        catch (const std::exception& e)
        {
            m_ostr << "Error: " << e.what() << '\n';
        }

        istr.clear();
        istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
	while (size > m_maxOperations || size < 2);
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


void FunctionCalculator::runAction(Action action, std::istringstream& iss, std::istream& istr)
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
        case Action::Help:         checkEndOfInput(iss);help();                     break;
        case Action::Exit:         checkEndOfInput(iss);exit();                     break;
		case Action::Read:         read(iss);                  break;
        //case Action::Iden:          unaryFunc<Identity>();      break;
        //case Action::Tran:          unaryFunc<Transpose>();      break;
        case Action::Scal:         checkMaxOperations(); unaryWithIntFunc<Scalar>(iss); break;
        case Action::Resize:       checkEndOfInput(iss);resize(istr);                   break;
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
