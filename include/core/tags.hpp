#pragma once

#include "core/errors/errors.hpp"

#include <functional>
#include <string>

namespace CXTPL {

namespace core {

typedef std::function<
  outcome::result<void, CXTPL::core::errors::GeneratorErrorExtraInfo>(
  std::string&, const std::string&, const std::string&)> tag_callback;

struct SingleTag {
  std::vector<const char*> opening;
  const char* attrs;
  std::vector<const char*> closing;
};

struct PairTag {
  SingleTag open_tag;
  SingleTag close_tag;
  tag_callback callback;
};

} // namespace core

} // namespace CXTPL
