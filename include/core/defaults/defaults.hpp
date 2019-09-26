#pragma once

#include "core/tags.hpp"

/// \note may be platform dependant ("\r", "\t", "\n", ...)
#define CXTPL_NEWLINE "\n"

#define CXTPL_TAG_OPENING "[["
#define CXTPL_TAG_CLOSING "]]"

namespace CXTPL {

namespace core {

namespace defaults {

struct DefaultTags {
  static const char* openTagStart;

  /// \example
  /// `[[~` means `start execution of C++ code while parsing template`.
  /// Requires `~]]` as closing tag.
  /// `~]]` means `end execution of C++ code while parsing template`
  static const PairTag code_block;

  /// \example
  /// `[[~]]` means `start execution of C++ code while parsing template`.
  /// Requires newline (`\n`) as closing tag.
  static const PairTag code_line;

  /// \example
  /// `[[+` means `add result of execution of C++ code
  /// to output while parsing template`.
  /// Result must be string. Requires `+]]` as closing tag.
  static const PairTag code_append_raw;

  /// \example
  /// `[[*` means `add result of execution of C++ code
  /// to output while parsing template`.
  /// Result will be converted to string (just wrapped in std::to_string).
  /// Requires `*]]` as closing tag.
  static const PairTag code_append_as_string;
};

} // namespace defaults

} // namespace core

} // namespace CXTPL
