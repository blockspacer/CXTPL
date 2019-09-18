#pragma once

#include <string>
#include <functional>

namespace CXTPL {

namespace cpp_codegen {
struct CodeGenerator {
  typedef std::function<std::string(const std::string&,
    const std::string&)> callback;

  /// \brief outputs same code
  static callback appendToVariable;

  /// \brief outputs code inside std::to_string
  static callback appendToVariableAsString;

  /// \brief executes same code
  static callback executeCodeLine;

  /// \brief executes same code
  static callback executeCodeMultiline;

  /// \brief outputs code inside R"raw(" and ")raw"
  static callback appendToVariableAsRawText;
};
} // namespace cpp_codegen

} // namespace CXTPL
