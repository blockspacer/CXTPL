// see https://github.com/bisegni/chaosframework/blob/master/chaos/common/script/cling/CPPScriptVM.cpp
// see https://github.com/galsasson/ofxCling
// see https://github.com/0xfd000000/qling/blob/22e56c4be0bbccb1d0437f610bfa37374b44b87f/qling/qling.cpp

#include <iostream>
#include <iterator>
#include <exception>
#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/program_options.hpp>
#include <boost/utility/in_place_factory.hpp>

#include "core/errors/errors.hpp"
#include "core/CXTPL.hpp"

namespace po = boost::program_options;

// A helper function to simplify the main part.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

int main(int argc, const char* const* argv) {
  using namespace ::CXTPL::core::errors;

  std::cout << "main..." << "\n";

  CXTPL::core::Generator template_engine;

  std::cout << "CXTPL::core::Generator..." << "\n";

  /*template_engine.tags().code_append_raw.open_tag = CXTPL::core::SingleTag{
    CXTPL_TAG_OPENING "sadasd", "", CXTPL_TAG_CLOSING,
  };*/

  const char* input
#ifdef NDEBUG
    = "";
#else
    = R"raw(

  start!

<CX=> // parameters begin

const std::string generator_path = "somepath";

std::vector<std::string> generator_includes{"someinclude"};

// parameters end
/* no newline, see CX=l */ <=CX><CX=l>
// This is generated file. Do not modify directly.
// Path to the code generator: <CX=r> generator_path <=CX>.

<CX=l> for(const auto& fileName: generator_includes) {
<CX=r> fileName /* CX=r used to append to cxtpl_output */ <=CX>
<CX=l> } // end for

  end!

)raw";
#endif

  const outcome::result<std::string, GeneratorErrorExtraInfo> genResult
    = template_engine.generate(input);

  std::cout << "checks..." << "\n";

  if(genResult.has_error()) {
    if(genResult.error().ec == GeneratorError::EMPTY_INPUT) {
      ///\note assume not error, just empty file
      printf("WARNING: empty string as Generator input\n");
      // TODO: skip empty file here
    } else {
      std::cout << "=== ERROR ===\n"
        << "ERROR message: " <<
        make_error_code(genResult.error().ec).message() << "\n"
        << "ERROR category: " <<
        " " << make_error_code(genResult.error().ec).category().name() << "\n"
        << "ERROR info: " <<
        " " << genResult.error().extra_info << "\n"
        << "input data: " << input;
        // TODO: file path here
      return EXIT_FAILURE;
    }
  } else {
    if(genResult.has_value()) {
      std::cout << "genResult:" << genResult.value().c_str() << "\n";
    }
  }

  std::vector<std::string> in_args;
  std::vector<std::string> out_args;
  int threads_arg;

  try {
    const char* help_arg_name = "help";
    const char* in_arg_name = "in,I";
    const char* out_arg_name = "out,O";
    const char* threads_arg_name = "threads,J";

    po::options_description desc("Allowed options");

    desc.add_options()
      (help_arg_name, "produce help message")
      (threads_arg_name, po::value(&threads_arg)->default_value(2), "number of threads")
      (in_arg_name, po::value(&in_args)->multitoken(), "template files")
      (out_arg_name, po::value(&out_args)->multitoken(), "where to place C++ code files generated from template");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count(help_arg_name)) {
      std::cout << desc << "\n";
      return EXIT_SUCCESS;
    }

    if (in_args.empty()) {
      std::cerr << "ERROR: no input files.\n";
      return EXIT_FAILURE;
    }

    if (out_args.empty()) {
      std::cerr << "ERROR: no output files.\n";
      return EXIT_FAILURE;
    }

    if (in_args.size() != out_args.size()) {
      std::cerr << "ERROR: number of input files "
                   "must be same as number of output files.\n";
      std::cout << "inputs: " << "\n";
      for(const auto& it: in_args) {
        std::cout << " + " << it << "\n";
      }

      std::cout << "outputs: " << "\n";
      for(const auto& it: out_args) {
        std::cout << " + " << it << "\n";
      }
      return EXIT_FAILURE;
    }
  }
  catch(std::exception& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    return EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "ERROR: Exception of unknown type!\n";
    return EXIT_FAILURE;
  }

  auto chrono_then = std::chrono::steady_clock::now();

  std::cout << "in_arg: " << "\n";
  for(const auto& it: in_args) {
    std::cout << " + " << it << "\n";
  }

  std::cout << "out_arg: " << "\n";
  for(const auto& it: out_args) {
    std::cout << " + " << it << "\n";
  }

  long int diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();
  long int diff_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();
  std::cerr << "Done in : " << diff_ms << " milliseconds (" << diff_ns << " nanoseconds)\n";
  return EXIT_SUCCESS;
}
