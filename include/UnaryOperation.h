#pragma once

#include "Operation.h"

#include <memory>


class UnaryOperation : public Operation
{
public:
    UnaryOperation();
    int inputCount() const override;
};
