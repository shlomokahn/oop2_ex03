#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iosfwd>
#include <optional>
#include <iostream>
#include <fstream> 
#include <sstream>

class Operation;


class FunctionCalculator
{
public:
    FunctionCalculator( std::ostream& ostr);
    void run(std::istream& istr);
    void run();

private:
    void eval(std::istringstream& iss, std::istream& istr);
    void del(std::istringstream& iss);
    void help();
    void exit();
    void read(std::istringstream& iss);

    template <typename FuncType>
    void binaryFunc(std::istringstream& iss)
    {
        if (auto f0 = readOperationIndex(iss), f1 = readOperationIndex(iss); f0 && f1)
        {
            m_operations.push_back(std::make_shared<FuncType>(m_operations[*f0], m_operations[*f1]));
        }
    }

    template <typename FuncType>
    void unaryFunc()
    {
    	m_operations.push_back(std::make_shared<FuncType>());
	}
    template <typename FuncType>
    void unaryWithIntFunc(std::istringstream& iss)
    {
        int i = 0;
        iss >> i;
        m_operations.push_back(std::make_shared<FuncType>(i));
    }
    void printOperations() const;

    enum class Action
    {
        Invalid,
        Eval,
        Iden,
        Tran,
        Scal,
        Sub,
        Add,
        Mul,
        Comp,
        Del,
        Help,
        Exit,
        Read,
    };

    struct ActionDetails
    {
        std::string command;
        std::string description;
        Action action;
    };

    using ActionMap = std::vector<ActionDetails>;
    using OperationList = std::vector<std::shared_ptr<Operation>>;

    const ActionMap m_actions;
    OperationList m_operations;
    bool m_running = true;
    std::ostream& m_ostr;

    std::optional<int> readOperationIndex(std::istringstream& iss) const;
    Action readAction(std::istringstream& iss) const;

    void runAction(Action action, std::istringstream& iss, std::istream& istr);

    ActionMap createActions() const;
    OperationList createOperations() const ;
};
