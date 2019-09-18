#include "core/CXTPL.hpp"

#include "core/defaults/defaults.hpp"
#include "codegen/cpp/cpp_codegen.hpp"
#include "core/errors/errors.hpp"

#include <iostream>
#include <sstream>
#include <string>

//#define DEBUG_OUTPUT 1

#if defined(DEBUG_OUTPUT)
static std::string dbgOut;
#endif // DEBUG_OUTPUT

using namespace CXTPL::core::errors;

namespace CXTPL {

namespace core {

namespace {

static const std::string kOutVarName = "output";

static bool startsWith(const std::string& in, const std::string& prefix) {
  return !in.compare(0, prefix.size(), prefix);
};

static std::string removePrefix(const std::string& from,
    const std::string& prefix) {
  return from.substr( prefix.size(), from.size() );
};

struct EncloseTagResult {
  std::string srcAfterTagCode;
  std::string tagCode;
  std::string::size_type close_pos;
};

static outcome::result<EncloseTagResult, GeneratorErrorExtraInfo> encloseTag(
    const std::string& processStr, const std::string& startTag,
    const std::string& closeTag) {
  std::string::size_type close_pos = processStr.find(closeTag);
  std::string srcAfterTagCode;
  std::string tagCode;

  if(close_pos == std::string::npos) {
    std::string error_details;
    error_details += "can`t find closing tag ";
    error_details += closeTag;
    error_details += " after starting tag ";
    error_details += startTag;
    error_details += " (" __FILE__ ")";
    return GeneratorErrorExtraInfo{
      //make_error_code(std::errc::no_such_file_or_directory),
      GeneratorError::UNCLOSED_TAG,
      std::move(error_details)};
  }

  tagCode = processStr.substr( 0, close_pos );
  srcAfterTagCode = processStr.substr(close_pos + closeTag.length(),
    processStr.length() );

  return EncloseTagResult{
    srcAfterTagCode,
    tagCode,
    close_pos};
};

static std::string tag_attr_with_closing(const SingleTag& tag) {
  return std::string{tag.attrs} + tag.closing;
}

static std::string tag_to_string(const SingleTag& tag) {
  return std::string{tag.opening} + tag.attrs + tag.closing;
}

static outcome::result<void, GeneratorErrorExtraInfo> handleTag(std::string& str, const PairTag& tag,
    std::string& resultStr) {
  const std::string start_tag_str = tag_to_string(tag.open_tag);
  const std::string close_tag_str = tag_to_string(tag.close_tag);

  outcome::result<EncloseTagResult, GeneratorErrorExtraInfo> closedTagResult
    = encloseTag(str, start_tag_str, close_tag_str);
  EncloseTagResult closedTag = OUTCOME_TRYX(closedTagResult);

  str = closedTag.srcAfterTagCode;
  tag.callback(resultStr, closedTag.tagCode, kOutVarName);
  return outcome::success();
}

static outcome::result<void, GeneratorErrorExtraInfo> handleTagStart(std::string& str,
    const GeneratorTags& tags, std::string& resultStr) {
  PairTag found_tag;

  if(const auto prefix
      = tag_attr_with_closing(tags.code_line.open_tag);
      startsWith(str, prefix)) {
    str = removePrefix( str, prefix );
    found_tag = tags.code_line;
    return handleTag(str, found_tag, resultStr);
  } else if(const auto prefix
      = tag_attr_with_closing(tags.code_block.open_tag);
      startsWith(str, prefix)) {
    str = removePrefix( str, prefix );
    found_tag = tags.code_block;
    return handleTag(str, found_tag, resultStr);
  } else if(const auto prefix
      = tag_attr_with_closing(tags.code_append_raw.open_tag);
      startsWith(str, prefix)) {
    str = removePrefix( str, prefix );
    found_tag = tags.code_append_raw;
    return handleTag(str, found_tag, resultStr);
  } else if(const auto prefix
      = tag_attr_with_closing(tags.code_append_as_string.open_tag);
      startsWith(str, prefix)) {
    str = removePrefix( str, prefix );
    found_tag = tags.code_append_as_string;
    return handleTag(str, found_tag, resultStr);
  }

  // unknown tag
  std::string error_details;
  error_details += "can`t find closing tag ";
  error_details += tag_to_string(found_tag.close_tag);
  error_details += " after tag ";
  error_details += tag_to_string(found_tag.open_tag);
  error_details += " (" __FILE__ ")";
  return GeneratorErrorExtraInfo{
    //make_error_code(std::errc::no_such_file_or_directory),
    GeneratorError::UNCLOSED_TAG,
    std::move(error_details)};
}

#if !defined(NDEBUG)
static outcome::result<void, GeneratorErrorExtraInfo>
    performDebugChecks(const GeneratorTags& tags) {
  bool isValid =
    tags.code_line.open_tag.opening == tags.openTagStart
    && tags.code_block.open_tag.opening == tags.openTagStart
    && tags.code_append_raw.open_tag.opening == tags.openTagStart
    && tags.code_append_as_string.open_tag.opening == tags.openTagStart;

  if(!isValid) {
    std::string error_details;
    error_details += "all opening tags must start with ";
    error_details += tags.openTagStart;
    error_details += " (" __FILE__ ")";
    return GeneratorErrorExtraInfo{
      //make_error_code(std::errc::no_such_file_or_directory),
      GeneratorError::UNKNOWN_TAG,
      std::move(error_details)};
  }

  return outcome::success();
}
#endif

static outcome::result<std::string, GeneratorErrorExtraInfo> generateFromString(const char* input,
    const GeneratorTags& tags) {
#if !defined(NDEBUG)
  OUTCOME_TRY(performDebugChecks(tags));
#endif

  std::string processStr = input;
  if(processStr.empty()) {
    std::string error_details = "empty string as input";
    error_details += " (" __FILE__ ")";
    return GeneratorErrorExtraInfo{
      //make_error_code(std::errc::no_such_file_or_directory),
      GeneratorError::EMPTY_INPUT,
      std::move(error_details)};
  }

  std::string resultStr;

  const auto max_iterations = std::numeric_limits<size_t>::max();
  for(size_t i = 0; i < max_iterations; i++)
  {
    std::string::size_type pos = processStr.find(tags.openTagStart);
    if(pos != std::string::npos) {
      resultStr += cpp_codegen::CodeGenerator::
        appendToVariableAsRawText(processStr.substr( 0, pos ), kOutVarName);
      pos += std::string{tags.openTagStart}.size();

#if defined(DEBUG_OUTPUT)
  dbgOut += "___[[kOpenTagStart]]___";
  dbgOut += processStr.substr( 0, pos );
#endif // DEBUG_OUTPUT

      processStr = processStr.substr( pos, processStr.size() );
      outcome::result<void, GeneratorErrorExtraInfo> result = handleTagStart(processStr, tags, resultStr);
      OUTCOME_TRY(result);
    } else {
      // remainder
      resultStr += cpp_codegen::CodeGenerator::
        appendToVariableAsRawText(processStr.substr( 0, processStr.size() ),
          kOutVarName);
      break;
    }
  }

#if defined(DEBUG_OUTPUT)
  std::cout << "dbgOut = \n" << dbgOut << std::endl;
  dbgOut.clear();
#endif // DEBUG_OUTPUT

  return resultStr;
}

} // namespace

#if defined CXTPL_FS
std::string Generator::generate(const fs::path &template_filepath) noexcept
{
  return generateFromString("", tags())
}
#endif // CXTPL_FS

outcome::result<std::string, GeneratorErrorExtraInfo> Generator::generate(const char* template_source) noexcept
{
  return generateFromString(template_source, tags());
}

GeneratorTags& Generator::tags() noexcept
{
  return GeneratorTags_;
}

} // namespace core

} // namespace CXTPL
