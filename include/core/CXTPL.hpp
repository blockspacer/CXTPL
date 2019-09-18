#pragma once

#include <functional>
#include <memory>

#if defined CXTPL_FS
// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if __has_include(<filesystem>)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#endif

#include "core/defaults/defaults.hpp"
#include "core/tags.hpp"
#include "core/errors/errors.hpp"

namespace CXTPL {

namespace core {

#if defined CXTPL_FS
namespace fs = std::experimental::filesystem;
#endif

struct GeneratorTags {
  const char* openTagStart =
    defaults::DefaultTags::openTagStart;

  PairTag code_block
    = defaults::DefaultTags::code_block;

  PairTag code_line
    = defaults::DefaultTags::code_line;

  PairTag code_append_raw
    = defaults::DefaultTags::code_append_raw;

  PairTag code_append_as_string
    = defaults::DefaultTags::code_append_as_string;
};

/// \brief used to generate C++ code from template
class Generator {
 public:
#if defined CXTPL_FS
  /// \brief used to generate C++ code from .cxtpl template file
  std::string generate(const fs::path& template_filepath) noexcept;
#endif

  /// \brief used to generate C++ code from template string
  outcome::result<std::string, errors::GeneratorErrorExtraInfo> generate(const char* template_source) noexcept;

  /// \brief used to change parsed tags
  GeneratorTags& tags() noexcept;

 private:
  GeneratorTags GeneratorTags_;
};

} // namespace core

} // namespace CXTPL
