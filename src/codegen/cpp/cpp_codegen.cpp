#include "codegen/cpp/cpp_codegen.hpp"

namespace CXTPL {

namespace cpp_codegen {

static const char* cpp_newline = R"outraw(
)outraw";

CodeGenerator::callback CodeGenerator::appendToVariable
  = [](const std::string& text, const std::string& outVarName) {
  std::string result;
  result += cpp_newline;
  result += outVarName;
  result += cpp_newline;
  result += R"outraw( += )outraw";
  result += text;
  result += R"outraw( ; )outraw";
  result += cpp_newline;
  return result;
};

CodeGenerator::callback CodeGenerator::appendToVariableAsString
  = [](const std::string& text, const std::string& outVarName) {
  std::string src;
  src += cpp_newline;
  src += R"outraw( std::to_string( )outraw"; // start to_string
  src += text;
  src += R"outraw( ) )outraw"; // end to_string
  src += cpp_newline;
  return appendToVariable(src, outVarName);
};

CodeGenerator::callback CodeGenerator::executeCodeLine
  = [](const std::string& text, const std::string& /*outVarName*/) {
  return text;
};

CodeGenerator::callback CodeGenerator::executeCodeMultiline
  = [](const std::string& text, const std::string& /*outVarName*/) {
  return text;
};

CodeGenerator::callback CodeGenerator::appendToVariableAsRawText
  = [](const std::string& text, const std::string& outVarName) {
  std::string src;
  src += cpp_newline;
  src += R"outraw(R"raw()outraw"; // start R"raw(
  src += text;
  src += R"outraw()raw")outraw";  // end )raw"
  src += cpp_newline;
  return cpp_codegen::CodeGenerator::
    appendToVariable(src, outVarName);
};

} // namespace cpp_codegen

} // namespace CXTPL
