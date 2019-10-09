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

#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"

using namespace CXTPL::core::errors;

namespace CXTPL {

namespace core {

namespace {

static const std::string kOutVarName = "cxtpl_output";

static base::string16 shiftPos(const base::string16& from,
  Generator::Position& pos, size_t start, size_t end) {
  pos.index += start;
  return from.substr( start, end - start);
}

static base::string16 removePrefix(const base::string16& from,
    const std::string& prefix, Generator::Position& pos) {
  return shiftPos( from, pos, prefix.size(), from.size() );
};

static const std::pair<long long, long long>
  highlight_visible_offset{20LL, 20LL};

static const char highlight_fill_symbol = ' ';

static long long getLineNum(
    const base::string16& original_str,
    const Generator::Position& curPos)
{
  long long result;
  /// \note we count lines from 1
  result = 1 + std::count(original_str.begin(), original_str.end(), '\n');
  return result;
}

static base::string16 highlightTextPos(
  const base::string16& original_str,
  const Generator::Position& curPos)
{
    base::string16 highlighted;
    highlighted += base::ASCIIToUTF16("\n");
    long long pos_index = std::min(
      std::max(0LL, static_cast<long long>(curPos.index)),
      static_cast<long long>(original_str.size()));
    long long start_highlight =
      std::max(0LL, pos_index - highlight_visible_offset.first);
    long long end_highlight =
      std::min(pos_index + highlight_visible_offset.second,
        static_cast<long long>(original_str.size()));

    const base::string16 visible_original_start
      = original_str.substr(
        static_cast<size_t>(start_highlight),
        static_cast<size_t>(pos_index - start_highlight));
    highlighted += visible_original_start;

    highlighted += base::ASCIIToUTF16("\n");

    std::string::size_type end_newline_pos
      = visible_original_start.rfind(base::ASCIIToUTF16("\n"));
    if(end_newline_pos == std::string::npos) {
      end_newline_pos = 0;
    }
    const size_t columnNum
      = visible_original_start.size() - end_newline_pos;

    long long lineNum
      = getLineNum(visible_original_start, curPos);

    const size_t fill_count = columnNum;
    highlighted.insert(highlighted.end(),
      fill_count, highlight_fill_symbol);
    highlighted += base::ASCIIToUTF16("^<==[error here]");
    highlighted += base::ASCIIToUTF16("[line: ");
    highlighted += base::ASCIIToUTF16(std::to_string(lineNum));
    highlighted += base::ASCIIToUTF16("]");
    highlighted += base::ASCIIToUTF16("[column: ");
    highlighted += base::ASCIIToUTF16(std::to_string(columnNum));
    highlighted += base::ASCIIToUTF16("]");
    highlighted += base::ASCIIToUTF16("\n");
    highlighted.insert(
      highlighted.end(), fill_count, highlight_fill_symbol);
    highlighted += base::ASCIIToUTF16("^");
    highlighted += base::ASCIIToUTF16("\n");
    highlighted.insert(
      highlighted.end(), fill_count, highlight_fill_symbol);
    highlighted += base::ASCIIToUTF16("^");
    highlighted += base::ASCIIToUTF16("\n");

    const base::string16 visible_original_end
      = original_str.substr(
      static_cast<size_t>(pos_index),
      static_cast<size_t>(end_highlight - pos_index));
    highlighted += visible_original_end;

    highlighted += base::ASCIIToUTF16("\n");
    return highlighted;
}

struct ErrorDetails {
  static std::string UnknownTag(const base::string16& original_str,
      const Generator::Position& curPos,
      const GeneratorTags& supported_tags)
  {
    std::string error_details;
    error_details += "can`t find known tag after opening tag ";
    error_details += supported_tags.openTagStart;
    error_details += " (" __FILE__ ")\n";
    error_details += base::UTF16ToUTF8(
      highlightTextPos(original_str, curPos));
    return error_details;
  }

  static std::string UnclosedTag(const base::string16& original_str,
      const Generator::Position& curPos,
      const std::string& startTag,
      const std::string& closeTag)
  {
    std::string error_details;
    error_details += "can`t find closing tag ";
    error_details += closeTag;
    error_details += " after starting tag ";
    error_details += startTag;
    error_details += " (" __FILE__ ")";
    error_details += base::UTF16ToUTF8(
      highlightTextPos(original_str, curPos));
    return error_details;
  }
};

static std::string tag_attr_with_closing(const SingleTag& tag,
  const size_t closeIndex = 0)
{
  /// \todo support closeIndex != 0
  DCHECK(closeIndex == 0);

  return std::string{tag.attrs} + tag.closing.at(closeIndex);
}

static std::string tag_to_string(const SingleTag& tag,
  const size_t openIndex = 0, const size_t closeIndex = 0)
{
  /// \todo support closeIndex != 0
  DCHECK(closeIndex == 0);

  return std::string{
    tag.opening.at(openIndex)}
    + tag.attrs
    + tag.closing.at(closeIndex);
}

#if !defined(NDEBUG)
static outcome::result<void, GeneratorErrorExtraInfo>
    performDebugChecks(const GeneratorTags& supported_tags) {

  CHECK(supported_tags.code_line.open_tag.opening.size() == 1);
  CHECK(supported_tags.code_block.open_tag.opening.size() == 1);
  CHECK(supported_tags.code_append_raw.open_tag.opening.size() == 1);
  CHECK(supported_tags.code_append_as_string.open_tag.opening.size() == 1);
  CHECK(supported_tags.code_include.open_tag.opening.size() == 1);

  bool isTagOpeningsValid =
    supported_tags.code_line.open_tag.opening.at(0)
      == supported_tags.openTagStart
    && supported_tags.code_block.open_tag.opening.at(0)
      == supported_tags.openTagStart
    && supported_tags.code_append_raw.open_tag.opening.at(0)
      == supported_tags.openTagStart
    && supported_tags.code_append_as_string.open_tag.opening.at(0)
      == supported_tags.openTagStart
    && supported_tags.code_include.open_tag.opening.at(0)
      == supported_tags.openTagStart;

  if(!isTagOpeningsValid) {
    std::string error_details;
    error_details += "all opening tags must start with ";
    error_details += supported_tags.openTagStart;
    error_details += " (" __FILE__ ")";
    return GeneratorErrorExtraInfo{
      GeneratorError::UNKNOWN_TAG,
      std::move(error_details)};
  }

  return outcome::success();
}
#endif

} // namespace

#if defined CXTPL_FS
std::string Generator::generate(const fs::path &template_filepath) noexcept
{
  return generateFromString("", tags())
}
#endif // CXTPL_FS

outcome::result<std::string, GeneratorErrorExtraInfo>
  Generator::generate_from_ASCII(const char* template_source) noexcept
{
  DCHECK(base::IsStringASCII(template_source));
  original_str = base::ASCIIToUTF16(template_source);
  return generateFromString();
}

outcome::result<std::string, GeneratorErrorExtraInfo>
  Generator::generate_from_UTF16(base::StringPiece16 template_source) noexcept
{
  original_str = template_source.as_string();
  return generateFromString();
}

GeneratorTags& Generator::supported_tags() noexcept
{
  return GeneratorTags_;
}

void Generator::set_supported_tags(
  const GeneratorTags& tags) noexcept
{
  GeneratorTags_ = tags;
}

outcome::result<
  Generator::EncloseTagResult,
  GeneratorErrorExtraInfo>
    Generator::encloseTag(
      base::string16& processStr, Generator::Position& curPos,
      const std::string& startTag, const std::string& closeTag)
{
  std::string::size_type close_pos = processStr.find(
    base::ASCIIToUTF16(closeTag));
  base::string16 tagCode;

  if(close_pos == std::string::npos) {
    /// \note in normal mode following is valid code:
    ///   [[~ [[~ ~]] ~]]~]]~]]
    /// because `[[~` in the middle considered as C++ code
    /// and we don`t check last `~]]~]]~]]`
    /// (checks only for tag pairs because of performance reasons)
    return GeneratorErrorExtraInfo{
      GeneratorError::UNCLOSED_TAG,
      ErrorDetails::UnclosedTag(original_str, curPos,
        startTag, closeTag)};
  }

  tagCode = processStr.substr( 0, close_pos );

  /// \note modifies source string, gets text after tag
  processStr =
    shiftPos(processStr, curPos, close_pos + closeTag.length(),
      processStr.length());

  return Generator::EncloseTagResult{
    tagCode};
};

outcome::result<std::string, GeneratorErrorExtraInfo>
  Generator::generateFromString()
{
#if !defined(NDEBUG)
  OUTCOME_TRY(performDebugChecks(supported_tags()));
#endif

  Generator::Position curPos{-1};

  base::string16 processStr = original_str;
  if(processStr.empty()) {
    std::string error_details = "empty string as input";
    error_details += " (" __FILE__ ")";
    return GeneratorErrorExtraInfo{
      GeneratorError::EMPTY_INPUT,
      std::move(error_details)};
  }

  std::string resultStr;

  const auto max_iterations = std::numeric_limits<size_t>::max();

  for(size_t i = 0; i < max_iterations; i++)
  {
    std::string::size_type pos =
      processStr.find(base::ASCIIToUTF16(
        supported_tags().openTagStart));
    if(pos != std::string::npos) {
      const base::string16 subStrRawText =
        shiftPos(processStr, curPos, 0, pos );
      if(subStrRawText.empty()) {
        resultStr += "\n"; // otherwise we may comment out next line
      } else {
        resultStr += cpp_codegen::CodeGenerator::
          appendToVariableAsRawText(
            std::string{base::UTF16ToUTF8(subStrRawText)},
            kOutVarName);
      }
      pos += std::string{supported_tags().openTagStart}.size();

#if defined(DEBUG_OUTPUT)
  dbgOut += "___[[kOpenTagStart]]___";
  dbgOut += processStr.substr( 0, pos );
#endif // DEBUG_OUTPUT

      processStr =
        shiftPos(processStr, curPos, pos, processStr.size() );
      outcome::result<void, GeneratorErrorExtraInfo> result =
        handleTagStart(processStr, curPos, resultStr);
      OUTCOME_TRY(result);
    } else {
      // remainder
      base::string16 subStrRawText =
        shiftPos(processStr, curPos, 0, processStr.size());
      if(subStrRawText.empty()) {
        resultStr += "\n"; // separate code statements, comments, ...
      } else {
        resultStr += cpp_codegen::CodeGenerator::
          appendToVariableAsRawText(
            std::string{base::UTF16ToUTF8(subStrRawText)},
            kOutVarName);
      }
      break;
    }
  }

#if defined(DEBUG_OUTPUT)
  std::cout << "dbgOut = \n" << dbgOut << std::endl;
  dbgOut.clear();
#endif // DEBUG_OUTPUT

  return std::move(resultStr);
}

outcome::result<void, GeneratorErrorExtraInfo> Generator::handleTag(
    base::string16& str, const PairTag& tag,
    Generator::Position& curPos, std::string& resultStr) {
  const std::string start_tag_str = tag_to_string(tag.open_tag);

  outcome::result<
    void,
    CXTPL::core::errors::GeneratorErrorExtraInfo> cb_result
      = outcome::success();

  for(size_t i = 0; i < tag.close_tag.opening.size(); i++) {
    const std::string close_tag_str
      = tag_to_string(tag.close_tag, i);
    auto strToChange = str;
    auto posToChange = curPos;
    outcome::result<
      Generator::EncloseTagResult, GeneratorErrorExtraInfo>
        closedTagResult
          = encloseTag(strToChange, posToChange,
              start_tag_str, close_tag_str);

    /// \note skip failed attempts until it is not last attempt
    if(closedTagResult.has_error()
       && i < (tag.close_tag.opening.size() - 1)) {
      continue;
    }
    curPos = posToChange;
    str = strToChange;

    Generator::EncloseTagResult closedTag =
      OUTCOME_TRYX(closedTagResult);

    cb_result =
      tag.callback(resultStr,
        base::UTF16ToUTF8(closedTag.tagCode),
        kOutVarName);

    return cb_result;
  }

  return cb_result;
}

outcome::result<void, GeneratorErrorExtraInfo> Generator::
  handleTagStart(
    base::string16& str, Generator::Position& curPos,
    std::string& resultStr)
{
  PairTag found_tag;

  if(const auto prefix
      = tag_attr_with_closing(
          supported_tags().code_line.open_tag);
      base::StartsWith(str, base::ASCIIToUTF16(prefix),
        base::CompareCase::SENSITIVE)) {
    str = removePrefix( str, prefix, curPos );
    found_tag = supported_tags().code_line;
    return handleTag(str, found_tag, curPos, resultStr);
  } else if(const auto prefix
      = tag_attr_with_closing(
          supported_tags().code_block.open_tag);
      base::StartsWith(str, base::ASCIIToUTF16(prefix),
        base::CompareCase::SENSITIVE)) {
    str = removePrefix( str, prefix, curPos );
    found_tag = supported_tags().code_block;
    return handleTag(str, found_tag, curPos, resultStr);
  } else if(const auto prefix
      = tag_attr_with_closing(
          supported_tags().code_append_raw.open_tag);
      base::StartsWith(str, base::ASCIIToUTF16(prefix),
        base::CompareCase::SENSITIVE)) {
    str = removePrefix( str, prefix, curPos );
    found_tag = supported_tags().code_append_raw;
    return handleTag(str, found_tag, curPos, resultStr);
  } else if(const auto prefix
      = tag_attr_with_closing(
          supported_tags().code_append_as_string.open_tag);
      base::StartsWith(str, base::ASCIIToUTF16(prefix),
        base::CompareCase::SENSITIVE)) {
    str = removePrefix( str, prefix, curPos );
    found_tag = supported_tags().code_append_as_string;
    return handleTag(str, found_tag, curPos, resultStr);
  } else if(const auto prefix
      = tag_attr_with_closing(
          supported_tags().code_include.open_tag);
      base::StartsWith(str, base::ASCIIToUTF16(prefix),
        base::CompareCase::SENSITIVE)) {
    str = removePrefix( str, prefix, curPos );
    found_tag = supported_tags().code_include;
    return handleTag(str, found_tag, curPos, resultStr);
  }

  // unknown tag
  std::string error_details;
  error_details += "can`t find known tag after opening tag ";
  error_details += supported_tags().openTagStart;
  error_details += " (" __FILE__ ")";
  return GeneratorErrorExtraInfo{
    GeneratorError::UNCLOSED_TAG,
    ErrorDetails::UnknownTag(original_str,
      curPos, supported_tags())};
}

} // namespace core

} // namespace CXTPL
