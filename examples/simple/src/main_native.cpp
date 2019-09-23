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

  const outcome::result<std::string, GeneratorErrorExtraInfo> genResult
    = template_engine.generate(R"raw(
<CX=> // parameters begin

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
/* no newline */ <=CX><CX=l>
// This is generated file. Do not modify directly.
// Path to the code generator: <CX=r> generator_path <=CX>.

<CX=r> startHeaderGuard(headerGuard) /* no newline */ <=CX><CX=l>

<CX=l> for(const auto& fileName: generator_includes) {
<CX=r> fileName <=CX>
<CX=l> } // end for

#include <array>
#include <functional>
#include <memory>

namespace cxxctp {
namespace generated {

<CX=l>  { // startscope
<CX=l>    for(const auto& method: ReflectedBaseTypeclass->methods) {
<CX=l>      const size_t methodParamsSize = method->params.size();
<CX=l>      const bool needPrint = !method->isImplicit
<CX=l>          && !method->isOperator
<CX=l>          && !method->isCtor
<CX=l>          && !method->isDtor;
<CX=l>      if(needPrint) {
  template<>
<CX=r> method->isExplicitCtor ? "explicit " : "" <=CX><CX=l> /* no newline */
<CX=r> method->isConstexpr ? "constexpr " : "" <=CX><CX=l> /* no newline */
<CX=r> method->isStatic ? "static " : "" <=CX><CX=l> /* no newline */
<CX=r> method->returnType->getPrintedName() <=CX><CX=l> /* no newline */
<CX=r> " " <=CX><CX=l> /* no newline */
<CX=r> method->name <=CX><CX=l> /* no newline */
< <CX=r> BaseTypeclassName <=CX>, <CX=r> ImplTypeclassName <=CX> >
(const <CX=r> ImplTypeclassName <=CX>& data<CX=l> /* no newline */
<CX=l> if(methodParamsSize) {
<CX=r>   ", " <=CX><CX=l> /* no newline */
<CX=r> paramsFullDecls(method->params) <=CX><CX=l> /* no newline */
<CX=l> } // methodParamsSize
) <CX=l> /* no newline */
<CX=r> method->isNoExcept ? "noexcept " : "" <=CX><CX=l> /* no newline */
;
<CX=> /* newline */ <=CX>
<CX=l>      } // needPrint
<CX=l>    } // methods endfor
<CX=l>  } // endscope

/*template <>
<CX=r> ImplTypeclassName <=CX>& _tc_model_t<<CX=r> BaseTypeclassName <=CX>>::ref_concrete<<CX=r> ImplTypeclassName <=CX>>() noexcept;
*/

template<>
struct _tc_impl_t<<CX=r> ImplTypeclassName <=CX>, <CX=r> BaseTypeclassName <=CX>>
    : public _tc_model_t<<CX=r> BaseTypeclassName <=CX>> {
    typedef <CX=r> ImplTypeclassName <=CX> val_type_t;

    // Construct the embedded concrete type.
    template<typename... args_t>
    _tc_impl_t(args_t&&... args) noexcept : concrete(std::forward<args_t>(args)...) { }

    explicit _tc_impl_t(const <CX=r> ImplTypeclassName <=CX>& concrete_arg) noexcept
      : concrete(concrete_arg) {}

    /*const size_t getModelTypeIndex() const noexcept override final {
        return _tc_registry<<CX=r> BaseTypeclassName <=CX>>::
            getTypeIndex<<CX=r> ImplTypeclassName <=CX>>();
    }*/


<CX=l>  { // startscope
<CX=l>    for(const auto& method: ReflectedBaseTypeclass->methods) {
<CX=l>      const size_t methodParamsSize = method->params.size();
<CX=l>      const bool needPrint = !method->isImplicit
<CX=l>          && !method->isOperator
<CX=l>          && !method->isCtor
<CX=l>          && !method->isDtor;
<CX=l>      if(method->isTemplate()) {
template< <CX=r> templateParamsFullDecls(method->tplParams) <=CX> >
<CX=l>      } // method->isTemplate
<CX=l>      if(needPrint) {
<CX=r> method->isExplicitCtor ? "explicit " : "" <=CX><CX=l> /* no newline */
<CX=r> method->isConstexpr ? "constexpr " : "" <=CX><CX=l> /* no newline */
<CX=r> method->isStatic ? "static " : "" <=CX><CX=l> /* no newline */
<CX=r> method->returnType->getPrintedName() <=CX><CX=l> /* no newline */
<CX=r> " __" <=CX><CX=l> /* no newline */
<CX=r> method->name <=CX><CX=l> /* no newline */
(
<CX=r> paramsFullDecls(method->params) <=CX><CX=l> /* no newline */
) <CX=l> /* no newline */
<CX=r> method->isConst ? "const " : "" <=CX><CX=l> /* no newline */
<CX=r> method->isNoExcept ? "noexcept " : "" <=CX><CX=l> /* no newline */
override final
  {
    /// \note passes data, not ref
    return <CX=r> method->name <=CX><CX=l> /* no newline */
< <CX=r> BaseTypeclassName <=CX> ><CX=l> /* no newline */
(<CX=l> /* no newline */
concrete<CX=l> /* no newline */
<CX=r> methodParamsSize ? ", " : "" <=CX><CX=l> /* no newline */
<CX=r> paramsCallDecls(method->params) <=CX><CX=l> /* no newline */
);
  }
<CX=> /* newline */ <=CX>
<CX=l>      } // needPrint
<CX=l>    } // methods endfor
<CX=l>  } // endscope

    std::unique_ptr<
        _tc_model_t<<CX=r> BaseTypeclassName <=CX>>>
    clone() noexcept override final {
        // Copy-construct a new instance of _tc_impl_t on the heap.
        return std::make_unique<_tc_impl_t>(concrete);
    }

    std::unique_ptr<
        _tc_model_t<<CX=r> BaseTypeclassName <=CX>>>
    move_clone() noexcept override final {
        return std::make_unique<_tc_impl_t>(std::move(concrete));
    }

    <CX=r> ImplTypeclassName <=CX>* operator->() {
        return &concrete;
    }

  // Our actual data.
  <CX=r> ImplTypeclassName <=CX> concrete;
};

} // namespace cxxctp
} // namespace generated

<CX=r> endHeaderGuard(headerGuard) /* no newline */ <=CX><CX=l>

    )raw");

  auto chrono_then = std::chrono::steady_clock::now();
  long int diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();

  if(genResult.has_error()) {
    if(genResult.error().ec == GeneratorError::EMPTY_INPUT) {
      std::cout << "WARNING: genResult.has_error() EMPTY_INPUT" << std::endl;
    } else {
      std::cerr << "ERROR: genResult.has_error(): " << genResult.error().ec << std::endl;
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
