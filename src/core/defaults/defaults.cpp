#include "core/defaults/defaults.hpp"
#include "core/CXTPL.hpp"
//#include "core/defaults/tag_callbacks.hpp"

//#define CXTPL_ENABLE_FOLLY 1

#ifdef CXTPL_ENABLE_FOLLY
#include <folly/io/IOBufQueue.h>
#include <folly/FileUtil.h>
#include <folly/File.h>
#endif // CXTPL_ENABLE_FOLLY

#include <fstream>

#include "codegen/cpp/cpp_codegen.hpp"

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "base/single_thread_task_runner.h"
#include "base/stl_util.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_task_runner_handle.h"
#include "base/i18n/case_conversion.h"
#include "base/i18n/i18n_constants.h"
#include "base/i18n/icu_string_conversions.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_util.h"
#include "third_party/icu/source/common/unicode/ucnv.h"

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

// TODO: move fs to separate file https://github.com/jonathan-lemos/CloudSync/blob/165e93f2cc5e77b9e95615a91876c03a8cbd7114/fs/file.cpp
#ifdef CXTPL_ENABLE_FOLLY
static std::string read_file(const std::string& in_path) {
  folly::IOBufQueue buf;
  try {
    const fs::path in_abs_path = fs::absolute(in_path);
    XLOG(DBG9) << "(CXTPL) started reading file " << in_abs_path;
    try {
      if(!fs::exists(in_abs_path) || !fs::is_regular_file(in_abs_path)) {
          XLOG(ERR) << "(CXTPL) Can't find file " << in_abs_path;
          return "";
      }
    } catch (fs::filesystem_error& e) {
      XLOG(ERR) << "Failed to determine if \""
        << in_abs_path << "\" is a file" << e.what();
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
  try {
    if(!fs::exists(in_abs_path) || !fs::is_regular_file(in_abs_path)) {
        return "";
    }
  } catch (fs::filesystem_error& e) {
    //std::cerr << "Failed to determine if \""
    //  << in_abs_path << "\" is a file" << e.what();
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
  std::string cleanPath;
  // removing only leading & trailing spaces/tabs/etc.
  base::TrimString(filePathBetweenTags, " \t\n\r\f\v", &cleanPath);

  /*std::cout << "code_include_cb: cleanPath = "
    << cleanPath << std::endl;*/

  const auto file_contents = read_file(cleanPath);

  /*std::cout << "code_include_cb: file_contents = "
    << file_contents << std::endl;*/

  if(file_contents.empty()) {
#ifdef CXTPL_ENABLE_FOLLY
    XLOG(WARN) << "(CXTPL) Empty file " << cleanPath;
#endif // CXTPL_ENABLE_FOLLY
    return CXTPL::core::errors::GeneratorErrorExtraInfo{
      CXTPL::core::errors::GeneratorError::EMPTY_INPUT,
      "(CXTPL) Empty file " + cleanPath};
  }

  base::string16 clean_contents;
  CXTPL::core::defaults::ConvertResponseToUTF16(
    /* unknown encoding */ "",
    file_contents,
    &clean_contents);

#if 0
  /// \todo detect BOM https://www.puredevsoftware.com/blog/2017/11/17/text-encoding/
  std::string clean_contents =
          file_contents
          /// \note save in UTF without BOM
          /*// skip BOM
          .substr(4)*/
          ;
#endif

  /*std::cout << "code_include_cb: clean_contents = "
    << clean_contents << std::endl;*/

  CXTPL::core::Generator template_engine;

  const outcome::result
    <std::string,
     CXTPL::core::errors::GeneratorErrorExtraInfo>
    genResult
      = template_engine.generate_from_UTF16(
          clean_contents);

  std::string genResultStr = OUTCOME_TRYX(genResult);

  if(genResultStr.empty()) {
#ifdef CXTPL_ENABLE_FOLLY
    XLOG(WARN) << "(CXTPL) Empty generator output from file " << cleanPath;
#endif // CXTPL_ENABLE_FOLLY
    return CXTPL::core::errors::GeneratorErrorExtraInfo{
      CXTPL::core::errors::GeneratorError::EMPTY_INPUT,
      "(CXTPL) Empty generator output from file " + cleanPath};
  }

  /*std::cout << "code_include_cb: genResultStr = "
    << genResultStr << std::endl;*/

  result += genResultStr;

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
    SingleTag{
      {openTagStart},
      "~",
      {""}},
    SingleTag{
      {""},
      "~",
      {CXTPL_TAG_CLOSING}},
    &code_block_cb
  };

// start: [[~]]
// end: newline
const PairTag DefaultTags::code_line
  = PairTag {
    SingleTag{
      {openTagStart},
      "~",
      {CXTPL_TAG_CLOSING}},
    SingleTag{
      {CXTPL_NEWLINE, "\t", "\r", "\v", "\f"},
      "",
      {""}},
    &code_line_cb
  };

// start: [[+
// end: +]]
const PairTag DefaultTags::code_append_raw
  = PairTag {
      SingleTag{
        {openTagStart},
        "+",
        {""}},
      SingleTag{
        {""},
        "+",
        {CXTPL_TAG_CLOSING}},
      &code_append_raw_cb
    };

// start: [[*
// end: *]]
const PairTag DefaultTags::code_append_as_string
  = PairTag {
      SingleTag{
        {openTagStart},
        "*",
        {""}},
      SingleTag{
        {""},
        "*",
        {CXTPL_TAG_CLOSING}},
      &code_append_as_string_cb
    };

// start: [[include
// end: include]]
const PairTag DefaultTags::code_include
  = PairTag {
      SingleTag{
        {openTagStart},
        "include",
        {""}},
      SingleTag{
        {""},
        "include",
        {CXTPL_TAG_CLOSING}},
    &code_include_cb
};


struct BomMapping {
  base::StringPiece prefix;
  const char* charset;
};

// from https://github.com/blockspacer/skia-opengl-emscripten/blob/15d1ed4b15a5f53c4aa0af40ff92a075ac551a3c/src/chromium/net/proxy_resolution/pac_file_fetcher_impl.cc#L69
const BomMapping kBomMappings[] = {
    {"\xFE\xFF", "utf-16be"},
    {"\xFF\xFE", "utf-16le"},
    {"\xEF\xBB\xBF", "utf-8"},
};

bool ConvertToUTF16WithSubstitutions(base::StringPiece text,
                                     const char* charset,
                                     base::string16* output) {
  return base::CodepageToUTF16(
      text, charset, base::OnStringConversionError::SUBSTITUTE, output);
}

// Converts |bytes| (which is encoded by |charset|) to UTF16, saving the resul
// to |*utf16|.
// If |charset| is empty, then we don't know what it was and guess.
void ConvertResponseToUTF16(const std::string& charset,
                            const std::string& bytes,
                            base::string16* utf16) {
  if (charset.empty()) {
    // Guess the charset by looking at the BOM.
    base::StringPiece bytes_str(bytes);
    for (const auto& bom : kBomMappings) {
      if (bytes_str.starts_with(bom.prefix)) {
        std::cout << "detected charset "
          << bom.prefix << " " << bom.charset << std::endl;
        return ConvertResponseToUTF16(
            bom.charset,
            // Strip the BOM in the converted response.
            bytes.substr(bom.prefix.size()), utf16);
      }
    }

    // Otherwise assume ISO-8859-1 if no charset was specified.
    return ConvertResponseToUTF16(base::kCodepageLatin1, bytes, utf16);
  }

  DCHECK(!charset.empty());

  // Be generous in the conversion -- if any characters lie outside of |charset|
  // (i.e. invalid), then substitute them with U+FFFD rather than failing.
  ConvertToUTF16WithSubstitutions(bytes, charset.c_str(), utf16);
}

} // namespace defaults

} // namespace core

} // namespace CXTPL
