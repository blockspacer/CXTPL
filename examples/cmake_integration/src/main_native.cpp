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

int main(int argc, char* argv[]) {

  auto chrono_then = std::chrono::steady_clock::now();
  long int diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();

  std::string cxtpl_output;

  // template params
  std::string generator_path = "example.cxtpl";
  std::vector<std::string> generator_includes{
    R"raw(#include <iostream>)raw",
    R"raw(#include <cstring>)raw"
  };

#include "generated/example_includes.cxtpl.cpp"

  std::cout << "Result:" << std::endl;
  std::cout << cxtpl_output;

  long int diff_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();
  std::cout << "Done in : " << diff_ms
    << " milliseconds (" << diff_ns << " nanoseconds)" << std::endl;
  return EXIT_SUCCESS;
}
