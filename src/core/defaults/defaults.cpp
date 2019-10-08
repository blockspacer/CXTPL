#include "core/defaults/defaults.hpp"
#include "core/CXTPL.hpp"
//#include "core/defaults/tag_callbacks.hpp"

//#define CXTPL_ENABLE_FOLLY 1

#ifdef CXTPL_ENABLE_FOLLY
#include <folly/io/IOBufQueue.h>
#include <folly/FileUtil.h>
#include <folly/File.h>
#include <folly/logging/xlog.h>
#endif // CXTPL_ENABLE_FOLLY

#include <fstream>

#include "codegen/cpp/cpp_codegen.hpp"

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if __has_include(<filesystem>)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif // __has_include

#if __has_include(<filesystem>)
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif // __has_include

namespace {

#ifdef CXTPL_ENABLE_FOLLY
static std::string read_file(const std::string& in_path) {
  folly::IOBufQueue buf;
  try {
    const fs::path in_abs_path = fs::absolute(in_path);
    XLOG(DBG9) << "(CXTPL) started reading file " << in_abs_path;
    if(!fs::exists(in_abs_path) || !fs::is_regular_file(in_abs_path)) {
        XLOG(ERR) << "(CXTPL) Can't find file " << in_abs_path;
        return "";
    }
    auto in_file = std::make_unique<folly::File>(in_abs_path);
    while (in_file) {
      auto data = buf.preallocate(4000, 4000);
      auto rc = folly::readNoInt(in_file->fd(), data.first, data.second);
      if (rc < 0) {
        XLOG(ERR) << "(CXTPL) Read error=" << rc;
        in_file.reset();
        break;
      } else if (rc == 0) {
        // done
        in_file.reset();
        XLOG(DBG9) << "(CXTPL) Read EOF for " << in_abs_path;
        break;
      } else {
        buf.postallocate(rc);
      }
    }
  } catch (const std::system_error& ex) {
    XLOG(ERR) << "(CXTPL) ERROR: Could not open file " << in_path
      << " exception = " << folly::exceptionStr(ex);
    return "";
  }

  if(buf.empty() || buf.front()->empty()) {
    XLOG(WARNING) << "(CXTPL) WARNING: empty input from file " << in_path;
    return "";
  }

  auto queueToString = [](const folly::IOBufQueue& queue) {
    std::string out;
    queue.appendToString(out);
    return out;
  };

  return queueToString(buf);
}
#else
static std::string read_file(const std::string& file_path) {
  const fs::path in_abs_path = fs::absolute(file_path);
  if(!fs::exists(in_abs_path) || !fs::is_regular_file(in_abs_path)) {
      return "";
  }
  std::ifstream file_stream(file_path, std::ios::binary);
  if(!file_stream.is_open()) {
      // TODO: better error reporting
      printf("ERROR: can`t read from file %s\n", file_path.c_str());
      return "";
  }
  return std::string((std::istreambuf_iterator<char>(file_stream)),
                     std::istreambuf_iterator<char>());
}
#endif // CXTPL_ENABLE_FOLLY

// trim from left
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from left & right
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

static outcome::result<
  void, CXTPL::core::errors::GeneratorErrorExtraInfo>
    code_block_cb(std::string& result,
      const std::string& codeBetweenTags,
      const std::string& outVarName)
{
  result += CXTPL::cpp_codegen::CodeGenerator::
    executeCodeMultiline(codeBetweenTags, outVarName);
  return outcome::success();
}

static outcome::result<
  void, CXTPL::core::errors::GeneratorErrorExtraInfo>
    code_line_cb(std::string& result,
      const std::string& codeBetweenTags,
      const std::string& outVarName)
{
  result += CXTPL::cpp_codegen::CodeGenerator::
    executeCodeLine(codeBetweenTags, outVarName);
  return outcome::success();
}

static outcome::result<
  void, CXTPL::core::errors::GeneratorErrorExtraInfo>
    code_append_raw_cb(std::string& result,
      const std::string& codeBetweenTags,
      const std::string& outVarName)
{
  result += CXTPL::cpp_codegen::CodeGenerator::
    appendToVariable(codeBetweenTags, outVarName);
  return outcome::success();
}

static outcome::result<
  void, CXTPL::core::errors::GeneratorErrorExtraInfo>
    code_append_as_string_cb(std::string& result,
      const std::string& codeBetweenTags,
      const std::string& outVarName)
{
  result += CXTPL::cpp_codegen::CodeGenerator::
    appendToVariableAsString(codeBetweenTags, outVarName);
  return outcome::success();
}

static outcome::result<
  void, CXTPL::core::errors::GeneratorErrorExtraInfo>
    code_include_cb(std::string& result,
      const std::string& filePathBetweenTags,
      const std::string& outVarName)
{
  std::string cleanPath = filePathBetweenTags;
  // removing only leading & trailing spaces/tabs/etc.
  trim(cleanPath);

  const auto file_contents = read_file(cleanPath)
    // skip BOM
    .substr(3);
  if(file_contents.empty()) {
#ifdef CXTPL_ENABLE_FOLLY
    XLOG(WARN) << "(CXTPL) Empty file " << cleanPath;
#endif // CXTPL_ENABLE_FOLLY
    return CXTPL::core::errors::GeneratorErrorExtraInfo{
      CXTPL::core::errors::GeneratorError::EMPTY_INPUT,
      "(CXTPL) Empty file " + cleanPath};
  }

  CXTPL::core::Generator template_engine;

  const outcome::result
    <std::string,
     CXTPL::core::errors::GeneratorErrorExtraInfo>
    genResult
      = template_engine.generate(file_contents.c_str());

  std::string genResultStr = OUTCOME_TRYX(genResult);

  if(genResultStr.empty()) {
#ifdef CXTPL_ENABLE_FOLLY
    XLOG(WARN) << "(CXTPL) Empty generator output from file " << cleanPath;
#endif // CXTPL_ENABLE_FOLLY
    return CXTPL::core::errors::GeneratorErrorExtraInfo{
      CXTPL::core::errors::GeneratorError::EMPTY_INPUT,
      "(CXTPL) Empty generator output from file " + cleanPath};
  }

  result += CXTPL::cpp_codegen::CodeGenerator::
    executeCodeMultiline(genResultStr, outVarName);
  return outcome::success();
}

}

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
    &code_block_cb
  };

// start: [[~]]
// end: newline
const PairTag DefaultTags::code_line
  = PairTag {
    SingleTag{openTagStart, "~", CXTPL_TAG_CLOSING},
    SingleTag{CXTPL_NEWLINE, "", ""},
    &code_line_cb
  };

// start: [[+
// end: +]]
const PairTag DefaultTags::code_append_raw
  = PairTag {
      SingleTag{openTagStart, "+", ""},
      SingleTag{"", "+", CXTPL_TAG_CLOSING},
      &code_append_raw_cb
    };

// start: [[*
// end: *]]
const PairTag DefaultTags::code_append_as_string
  = PairTag {
      SingleTag{openTagStart, "*", ""},
      SingleTag{"", "*", CXTPL_TAG_CLOSING},
      &code_append_as_string_cb
    };

// start: [[include
// end: include]]
const PairTag DefaultTags::code_include
  = PairTag {
      SingleTag{openTagStart, "include", ""},
      SingleTag{"", "include", CXTPL_TAG_CLOSING},
      &code_include_cb
    };
} // namespace defaults

} // namespace core

} // namespace CXTPL
