#include "core/defaults/defaults.hpp"

#include "codegen/cpp/cpp_codegen.hpp"

namespace CXTPL {

namespace core {

namespace defaults {

const char* DefaultTags::openTagStart = CXTPL_TAG_OPENING "CX=";

// start: <CX=>
// end: <=CX>
const PairTag DefaultTags::code_block
  = PairTag {
    SingleTag{openTagStart, "", CXTPL_TAG_CLOSING},
    SingleTag{CXTPL_TAG_OPENING "=CX", "", CXTPL_TAG_CLOSING},
    [](std::string& result, const std::string& codeBetweenTags,
        const std::string& outVarName) {
      result += cpp_codegen::CodeGenerator::
        executeCodeMultiline(codeBetweenTags, outVarName);
    }
  };

// start: <CX=l>
// end: newline
const PairTag DefaultTags::code_line
  = PairTag {
    SingleTag{openTagStart, "l", CXTPL_TAG_CLOSING},
    SingleTag{CXTPL_NEWLINE, "", ""},
    [](std::string& result, const std::string& codeBetweenTags,
        const std::string& outVarName) {
      result += cpp_codegen::CodeGenerator::
        executeCodeLine(codeBetweenTags, outVarName);
    }
  };

// start: <CX=r>
// end: <=CX>
const PairTag DefaultTags::code_append_raw
  = PairTag {
      SingleTag{openTagStart, "r", CXTPL_TAG_CLOSING},
      code_block.close_tag,
      [](std::string& result, const std::string& codeBetweenTags,
          const std::string& outVarName) {
        result += cpp_codegen::CodeGenerator::
          appendToVariable(codeBetweenTags, outVarName);
      }
    };

// start: <CX=s>
// end: <=CX>
const PairTag DefaultTags::code_append_as_string
  = PairTag {
      SingleTag{openTagStart, "s", CXTPL_TAG_CLOSING},
      code_block.close_tag,
      [](std::string& result, const std::string& codeBetweenTags,
          const std::string& outVarName) {
        result += cpp_codegen::CodeGenerator::
          appendToVariableAsString(codeBetweenTags, outVarName);
      }
    };

} // namespace defaults

} // namespace core

} // namespace CXTPL
