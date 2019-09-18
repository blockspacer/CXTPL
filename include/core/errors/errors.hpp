#pragma once

#include "interations/outcome/error_utils.hpp"

#include <string>
#include <system_error>

namespace CXTPL {

namespace core {

namespace errors {

enum class GeneratorError {
  UNKNOWN_TAG,                      ///< unknown key type
  FILE_NOT_FOUND,                   ///< file not found
  FAILED_TO_READ_FILE,              ///< failed to read file
  FAILED_TO_WRITE_FILE,             ///< failed to read file
  EMPTY_INPUT,                      ///< no input data
  UNCLOSED_TAG,                     ///< starting tag without ending tag
};

// Error code + data related to a failure.
// Also causes ADL discovery to check this namespace.
struct GeneratorErrorExtraInfo
{
  GeneratorError ec;
  std::string extra_info;
};

} // namespace errors

} // namespace core

} // namespace CXTPL

OUTCOME_HPP_DECLARE_CATEGORY(CXTPL::core::errors::GeneratorError)
OUTCOME_HPP_DECLARE_ERROR(CXTPL::core::errors, GeneratorError)
OUTCOME_HPP_DECLARE_ERROR_EXTRA_INFO(CXTPL::core::errors, GeneratorErrorExtraInfo)
