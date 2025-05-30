#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iosfwd>
#include <optional>
#include <iostream>
#include <fstream> 
#include <sstream>
#include "inputStringStream.h"

class Operation;


class FunctionCalculator
{
public:
    FunctionCalculator( std::ostream& ostr);
    void run(std::istream& istr);
    void run();

private:
    void eval(inputStringStream& iss, std::istream& istr);
    void del(inputStringStream& iss);
    void help();
    void exit();
    void read(inputStringStream& iss);
    void resize(std::istream& istr);

	bool m_isFromFile = false;
    int m_maxOperations = 0;

    template <typename FuncType>
    void binaryFunc(inputStringStream& iss)
    {
        if (auto f0 = readOperationIndex(iss), f1 = readOperationIndex(iss); f0 && f1)
        {
            iss.checkEndOfInput();
            m_operations.push_back(std::make_shared<FuncType>(m_operations[*f0], m_operations[*f1]));
        }
    }

    template <typename FuncType>
    void unaryFunc()
    {
    	m_operations.push_back(std::make_shared<FuncType>());
	}
    template <typename FuncType>
    void unaryWithIntFunc(inputStringStream& iss)
    {
        int i = iss.getInt();
        iss.checkEndOfInput();
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
        Resize,
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

    std::optional<int> readOperationIndex(inputStringStream& iss) const;
    Action readAction(inputStringStream& iss) const;

    void runAction(Action action, inputStringStream& iss, std::istream& istr);
	void checkMaxOperations() const;

    ActionMap createActions() const;
    OperationList createOperations() const ;
};
