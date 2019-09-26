#include <iostream>
#include <iterator>
#include <exception>
#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if __has_include(<filesystem>)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif // __has_include

#include "core/errors/errors.hpp"
#include "core/CXTPL.hpp"

int main(int argc, char* argv[]) {

  using namespace ::CXTPL::core::errors;
  CXTPL::core::Generator template_engine;

  const std::string input = R"raw(
[[~ // parameters begin

const auto headerGuard
  = GetWithDefault<std::string>(cxtpl_params, "headerGuard", "");

const auto generator_path
  = GetWithDefault<std::string>(cxtpl_params, "generator_path", "");

std::vector<std::string> generator_includes
  = GetWithDefault<std::vector<std::string>>
      (cxtpl_params, "generator_includes", std::vector<std::string>{});

reflection::ClassInfoPtr ReflectedBaseTypeclass
  = GetWithDefault<reflection::ClassInfoPtr>
      (cxtpl_params, "ReflectedBaseTypeclass", nullptr);

std::string ImplTypeclassName
  = GetWithDefault<std::string>
      (cxtpl_params, "ImplTypeclassName", "");

std::string BaseTypeclassName
  = GetWithDefault<std::string>
      (cxtpl_params, "BaseTypeclassName", "");

// parameters end
/* no newline */ +]][[~]]
// This is generated file. Do not modify directly.
// Path to the code generator: [[+ generator_path +]].

[[+ startHeaderGuard(headerGuard) /* no newline */ +]][[~]]

[[~]] for(const auto& fileName: generator_includes) {
[[+ fileName +]]
[[~]] } // end for

#include <array>
#include <functional>
#include <memory>

namespace cxxctp {
namespace generated {

[[~]]  { // startscope
[[~]]    for(const auto& method: ReflectedBaseTypeclass->methods) {
[[~]]      const size_t methodParamsSize = method->params.size();
[[~]]      const bool needPrint = !method->isImplicit
[[~]]          && !method->isOperator
[[~]]          && !method->isCtor
[[~]]          && !method->isDtor;
[[~]]      if(needPrint) {
  template<>
[[+ method->isExplicitCtor ? "explicit " : "" +]][[~]] /* no newline */
[[+ method->isConstexpr ? "constexpr " : "" +]][[~]] /* no newline */
[[+ method->isStatic ? "static " : "" +]][[~]] /* no newline */
[[+ method->returnType->getPrintedName() +]][[~]] /* no newline */
[[+ " " +]][[~]] /* no newline */
[[+ method->name +]][[~]] /* no newline */
< [[+ BaseTypeclassName +]], [[+ ImplTypeclassName +]] >
(const [[+ ImplTypeclassName +]]& data[[~]] /* no newline */
[[~]] if(methodParamsSize) {
[[+   ", " +]][[~]] /* no newline */
[[+ paramsFullDecls(method->params) +]][[~]] /* no newline */
[[~]] } // methodParamsSize
) [[~]] /* no newline */
[[+ method->isNoExcept ? "noexcept " : "" +]][[~]] /* no newline */
;
[[~ /* newline */ +]]
[[~]]      } // needPrint
[[~]]    } // methods endfor
[[~]]  } // endscope

/*template <>
[[+ ImplTypeclassName +]]& _tc_model_t<[[+ BaseTypeclassName +]]>::ref_concrete<[[+ ImplTypeclassName +]]>() noexcept;
*/

template<>
struct _tc_impl_t<[[+ ImplTypeclassName +]], [[+ BaseTypeclassName +]]>
    : public _tc_model_t<[[+ BaseTypeclassName +]]> {
    typedef [[+ ImplTypeclassName +]] val_type_t;

    // Construct the embedded concrete type.
    template<typename... args_t>
    _tc_impl_t(args_t&&... args) noexcept : concrete(std::forward<args_t>(args)...) { }

    explicit _tc_impl_t(const [[+ ImplTypeclassName +]]& concrete_arg) noexcept
      : concrete(concrete_arg) {}

    /*const size_t getModelTypeIndex() const noexcept override final {
        return _tc_registry<[[+ BaseTypeclassName +]]>::
            getTypeIndex<[[+ ImplTypeclassName +]]>();
    }*/


[[~]]  { // startscope
[[~]]    for(const auto& method: ReflectedBaseTypeclass->methods) {
[[~]]      const size_t methodParamsSize = method->params.size();
[[~]]      const bool needPrint = !method->isImplicit
[[~]]          && !method->isOperator
[[~]]          && !method->isCtor
[[~]]          && !method->isDtor;
[[~]]      if(method->isTemplate()) {
template< [[+ templateParamsFullDecls(method->tplParams) +]] >
[[~]]      } // method->isTemplate
[[~]]      if(needPrint) {
[[+ method->isExplicitCtor ? "explicit " : "" +]][[~]] /* no newline */
[[+ method->isConstexpr ? "constexpr " : "" +]][[~]] /* no newline */
[[+ method->isStatic ? "static " : "" +]][[~]] /* no newline */
[[+ method->returnType->getPrintedName() +]][[~]] /* no newline */
[[+ " __" +]][[~]] /* no newline */
[[+ method->name +]][[~]] /* no newline */
(
[[+ paramsFullDecls(method->params) +]][[~]] /* no newline */
) [[~]] /* no newline */
[[+ method->isConst ? "const " : "" +]][[~]] /* no newline */
[[+ method->isNoExcept ? "noexcept " : "" +]][[~]] /* no newline */
override final
  {
    /// \note passes data, not ref
    return [[+ method->name +]][[~]] /* no newline */
< [[+ BaseTypeclassName +]] >[[~]] /* no newline */
([[~]] /* no newline */
concrete[[~]] /* no newline */
[[+ methodParamsSize ? ", " : "" +]][[~]] /* no newline */
[[+ paramsCallDecls(method->params) +]][[~]] /* no newline */
);
  }
[[~ /* newline */ +]]
[[~]]      } // needPrint
[[~]]    } // methods endfor
[[~]]  } // endscope

    std::unique_ptr<
        _tc_model_t<[[+ BaseTypeclassName +]]>>
    clone() noexcept override final {
        // Copy-construct a new instance of _tc_impl_t on the heap.
        return std::make_unique<_tc_impl_t>(concrete);
    }

    std::unique_ptr<
        _tc_model_t<[[+ BaseTypeclassName +]]>>
    move_clone() noexcept override final {
        return std::make_unique<_tc_impl_t>(std::move(concrete));
    }

    [[+ ImplTypeclassName +]]* operator->() {
        return &concrete;
    }

  // Our actual data.
  [[+ ImplTypeclassName +]] concrete;
};

} // namespace cxxctp
} // namespace generated

[[+ endHeaderGuard(headerGuard) /* no newline */ +]][[~]]

    )raw";

  const outcome::result<std::string, GeneratorErrorExtraInfo> genResult
    = template_engine.generate(input.c_str());

  auto chrono_then = std::chrono::steady_clock::now();
  long int diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();

  if(genResult.has_error()) {
    if(genResult.error().ec == GeneratorError::EMPTY_INPUT) {
      ///\note assume not error, just empty file
      std::cerr << "ERROR: empty string as Generator input\n";
      return EXIT_FAILURE;
    } else {
      std::cerr << "=== ERROR START ==="
        << std::endl;
      std::cerr << "ERROR message: " <<
        make_error_code(genResult.error().ec).message()
        << std::endl;
      std::cerr << "ERROR category: " <<
        " " << make_error_code(genResult.error().ec).category().name()
        << std::endl;
      std::cerr << "ERROR info: " <<
        " " << genResult.error().extra_info
        << std::endl;
      std::cerr << "input data: "
        /// \note limit to first 200 symbols
        << input.substr(0, std::min(200UL, input.size()))
        << "..." << std::endl;
      // TODO: file path here
      std::cerr << "=== ERROR END ==="
        << std::endl;
      return EXIT_FAILURE;
    }
  }

  std::cout << "genResult.value():" << std::endl;
  std::cout << genResult.value() << std::endl;

  long int diff_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();
  std::cout << "Done in : " << diff_ms
    << " milliseconds (" << diff_ns << " nanoseconds)" << std::endl;
  return EXIT_SUCCESS;
}
