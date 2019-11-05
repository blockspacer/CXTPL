#pragma once

#if defined(CLING_IS_ON)

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <condition_variable>

#include <cling/Interpreter/Interpreter.h>
#include <cling/Interpreter/Value.h>
#include "cling/Interpreter/CIFactory.h"
#include "cling/Interpreter/Interpreter.h"
#include "cling/Interpreter/InterpreterCallbacks.h"
//#include "cling/Interpreter/IncrementalExecutor.h"
//#include "cling/Interpreter/IncrementalParser.h"
#include "cling/Interpreter/Transaction.h"
#include "cling/Interpreter/Value.h"
#include "cling/Interpreter/CValuePrinter.h"
#include "cling/MetaProcessor/MetaProcessor.h"
#include <cling/Utils/Casting.h>
#include "cling/Interpreter/LookupHelper.h"
#include "cling/Utils/AST.h"
#include <cling/Interpreter/Interpreter.h>
#include <cling/Interpreter/Value.h>
#include "cling/Interpreter/CIFactory.h"
#include "cling/Interpreter/Interpreter.h"
#include "cling/Interpreter/InterpreterCallbacks.h"
#include "cling/Interpreter/Transaction.h"
#include "cling/Interpreter/Value.h"
#include "cling/Interpreter/CValuePrinter.h"
#include "cling/MetaProcessor/MetaProcessor.h"
#include <cling/Utils/Casting.h>
#include "cling/Interpreter/LookupHelper.h"
#include "cling/Utils/AST.h"

namespace cling_utils {

class InterpreterModule {
public:
    InterpreterModule(const std::string& id);

    ~InterpreterModule();

    void createInterpreter();

    void processCode(const std::string& code);

private:

    std::string id_;
    std::unique_ptr<cling::Interpreter> interpreter_;
    std::unique_ptr<cling::MetaProcessor> metaProcessor_;

    static std::vector<std::string> extra_args;
};

void add_default_cling_args(std::vector<std::string> &args);

} // namespace cling_utils

#endif // CLING_IS_ON
