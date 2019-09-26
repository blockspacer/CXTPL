#include "core/defaults/defaults.hpp"

#include "codegen/cpp/cpp_codegen.hpp"

namespace CXTPL {

namespace core {

namespace defaults {

const char* DefaultTags::openTagStart = CXTPL_TAG_OPENING;

// start: [[~
// end: ~]]
const PairTag DefaultTags::code_block
  = PairTag {
    SingleTag{openTagStart, "~", ""},
    SingleTag{"", "~", CXTPL_TAG_CLOSING},
    [](std::string& result, const std::string& codeBetweenTags,
        const std::string& outVarName) {
      result += cpp_codegen::CodeGenerator::
        executeCodeMultiline(codeBetweenTags, outVarName);
    }
  };

// start: [[~]]
// end: newline
const PairTag DefaultTags::code_line
  = PairTag {
    SingleTag{openTagStart, "~", CXTPL_TAG_CLOSING},
    SingleTag{CXTPL_NEWLINE, "", ""},
    [](std::string& result, const std::string& codeBetweenTags,
        const std::string& outVarName) {
      result += cpp_codegen::CodeGenerator::
        executeCodeLine(codeBetweenTags, outVarName);
    }
  };

// start: [[+
// end: +]]
const PairTag DefaultTags::code_append_raw
  = PairTag {
      SingleTag{openTagStart, "+", ""},
      SingleTag{"", "+", CXTPL_TAG_CLOSING},
      [](std::string& result, const std::string& codeBetweenTags,
          const std::string& outVarName) {
        result += cpp_codegen::CodeGenerator::
          appendToVariable(codeBetweenTags, outVarName);
      }
    };

// start: [[*
// end: *]]
const PairTag DefaultTags::code_append_as_string
  = PairTag {
      SingleTag{openTagStart, "*", ""},
      SingleTag{"", "*", CXTPL_TAG_CLOSING},
      [](std::string& result, const std::string& codeBetweenTags,
          const std::string& outVarName) {
        result += cpp_codegen::CodeGenerator::
          appendToVariableAsString(codeBetweenTags, outVarName);
      }
    };

} // namespace defaults

} // namespace core

} // namespace CXTPL
