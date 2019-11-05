#include "ClingInterpreterModule.hpp"

#if defined(CLING_IS_ON)

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

namespace cling_utils {

std::vector<std::string>
  InterpreterModule::extra_args;

InterpreterModule::InterpreterModule(const std::string &id)
  : id_(id)
{
    createInterpreter();
}

InterpreterModule::~InterpreterModule() {}

void InterpreterModule::processCode(const std::string& code) {
  cling::Value res; // Will hold the result of the expression evaluation.
  interpreter_->process(code.c_str(), &res);
}

void add_default_cling_args(std::vector<std::string> &args) {
    args.push_back("EmbedCling");
    args.push_back("-I.");
    args.push_back("-I../");

    args.push_back("--std=c++17");

    args.push_back("-I../cling-build/build/lib/clang/5.0.0/include");
    args.push_back("-I../cling-build/src/include/");
    args.push_back("-I../cling-build/build/include/");
    args.push_back("-I../cling-build/src/tools/clang/include/");
    args.push_back("-I../cling-build/build/tools/clang/include/");
    args.push_back("-I../cling-build/src/tools/cling/include/");

    // https://stackoverflow.com/a/30877725
    args.push_back("-DBOOST_SYSTEM_NO_DEPRECATED");
    args.push_back("-DBOOST_ERROR_CODE_HEADER_ONLY");

    for(const auto& it: InterpreterModule::extra_args) {
      args.push_back(it);
    }
}

void InterpreterModule::createInterpreter() {
    std::vector<std::string> args;
    add_default_cling_args(args);

    std::vector< const char* > interp_args;
    {
        std::vector< std::string >::const_iterator iarg;
        for( iarg = args.begin() ; iarg != args.end() ; ++iarg ) {
            interp_args.push_back(iarg->c_str());
        }
    }
    interpreter_ = std::make_unique<cling::Interpreter>(
                interp_args.size(), &(interp_args[0]), LLVMDIR/*, {}, false*/);
    interpreter_->AddIncludePath(".");
    interpreter_->AddIncludePath("../");
    interpreter_->enableDynamicLookup(true);
    metaProcessor_ = std::make_unique<cling::MetaProcessor>(*interpreter_, llvm::outs());

    interpreter_->process("#define CLING_IS_ON 1");
}

} // namespace cling_utils

#endif // CLING_IS_ON
