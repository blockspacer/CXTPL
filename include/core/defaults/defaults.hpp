#pragma once

#include "core/tags.hpp"

#include "base/strings/string16.h"
#include "base/strings/string_piece.h"

/// \note may be platform dependant ("\r", "\t", "\n", ...)
#define CXTPL_NEWLINE "\n"

#define CXTPL_TAG_OPENING "[["
#define CXTPL_TAG_CLOSING "]]"

namespace CXTPL {

namespace core {

namespace defaults {

bool ConvertToUTF16WithSubstitutions(base::StringPiece text,
                                     const char* charset,
                                     base::string16* output);

// Converts |bytes| (which is encoded by |charset|) to UTF16, saving the resul
// to |*utf16|.
// If |charset| is empty, then we don't know what it was and guess.
void ConvertResponseToUTF16(const std::string& charset,
                            const std::string& bytes,
                            base::string16* utf16);

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

  /// \example
  /// `[[include file/path/here include]]`
  static const PairTag code_include;
};

} // namespace defaults

} // namespace core

} // namespace CXTPL
