#include <iostream>
#include <iterator>
#include <exception>
#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

#include <gsl/gsl_util>

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if __has_include(<filesystem>)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif // __has_include

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/program_options.hpp>
#include <boost/utility/in_place_factory.hpp>

//#include <folly/io/async/ScopedEventBaseThread.h>
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/executors/GlobalExecutor.h>
#include <folly/Executor.h>
#include <folly/Try.h>
#include <folly/executors/Async.h>
#include <folly/executors/GlobalExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/Promise.h>
#include <folly/String.h>
#include <folly/FileUtil.h>
#include <folly/File.h>
#include <folly/io/IOBufQueue.h>
#include <folly/Optional.h>
#include <folly/Memory.h>
#include <folly/executors/GlobalExecutor.h>
#include <folly/io/async/EventBaseManager.h>
#include <folly/init/Init.h>
#include <folly/logging/Init.h>
#include <folly/logging/xlog.h>
#include <folly/logging/RateLimiter.h>
#include <folly/init/Init.h>
#include <folly/Singleton.h>
#include <folly/logging/Init.h>
#include <folly/portability/Config.h>
#include <folly/logging/StandardLogHandler.h>
#include <folly/Conv.h>
#include <folly/logging/Init.h>
#include <folly/logging/LogConfigParser.h>
#include <folly/logging/LogFormatter.h>
#include <folly/logging/FileHandlerFactory.h>
#include <folly/logging/StreamHandlerFactory.h>
#include <folly/logging/LogHandlerFactory.h>
#include <folly/logging/LogWriter.h>
#include <folly/logging/LoggerDB.h>
#include <folly/logging/StandardLogHandler.h>
#include <folly/logging/StandardLogHandlerFactory.h>
#include <folly/logging/xlog.h>
#include <folly/FileUtil.h>
#include <folly/Random.h>
#include <folly/ScopeGuard.h>
#include <folly/hash/SpookyHashV2.h>
#include <folly/json.h>
//#include <folly/experimental/TimerFDTimeoutManager.h>
//#include <folly/experimental/STTimerFDTimeoutManager.h>
#include <folly/io/async/test/Util.h>
//#include <folly/experimental/STTimerFDTimeoutManager.h>
//#include <folly/experimental/TimerFDTimeoutManager.h>
#include <folly/io/async/test/UndelayedDestruction.h>
#include <folly/executors/TimedDrivableExecutor.h>
#include <folly/Conv.h>
#include <folly/portability/GFlags.h>
#include <folly/ssl/Init.h>

#include "base/i18n/icu_util.h"

#include <glog/logging.h>

#if FOLLY_USE_SYMBOLIZER
#include <folly/experimental/symbolizer/SignalHandler.h> // @manual
#endif
#include <folly/portability/GFlags.h>

#include "base/strings/string16.h"
#include "base/strings/utf_string_conversions.h"

// boost log or
// #include <glog/logging.h>
#include <glog/logging.h>

#include "core/errors/errors.hpp"

#include "core/CXTPL.hpp"

#include "version.hpp"

#if defined(CLING_IS_ON)
#include "ClingInterpreterModule.hpp"

static std::string generator_file_contents;
#endif //CLING_IS_ON

namespace po = boost::program_options;

#if __has_include(<filesystem>)
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif // __has_include

static std::shared_ptr<folly::CPUThreadPoolExecutor> CPU_executor;

static std::vector<std::string> in_args;

static std::vector<std::string> out_args;

static boost::optional<std::string> srcdir_arg;

static boost::optional<std::string> resdir_arg;

static boost::optional<std::string> log_config;

static boost::optional<std::string> generator_path_arg;

static unsigned long threads_arg;

static fs::path srcdir_abs_path;

static fs::path resdir_abs_path;

static boost::optional<std::chrono::milliseconds> single_task_timeout;

  /// \note summary timeout for all files in one CPU_executor
static boost::optional<std::chrono::milliseconds> global_timeout;

#if defined(CLING_IS_ON)
std::unique_ptr<cling_utils::InterpreterModule> g_interp;
#endif // CLING_IS_ON

#if 0 // TODO: custom logger format
namespace {
class TestLogFormatter : public folly::LogFormatter {
 public:
  std::string formatMessage(
      const folly::LogMessage& logMessage,
      const folly::LogCategory* /* handlerCategory */) override {
    return "Test Formatter! " + logMessage.getMessage();
  }
};

class TestLogWriter : public folly::LogWriter {
 public:
  void writeMessage(folly::StringPiece buffer, uint32_t /* flags */ = 0)
      override {
    messages.push_back(buffer.str());
  }

  void flush() override {}

  std::vector<std::string> messages;

  bool ttyOutput() const override {
    return false;
  }
};

class TestHandlerFactory : public folly::LogHandlerFactory {
 public:
  explicit TestHandlerFactory(
      const std::shared_ptr<TestLogWriter> writer,
      const std::shared_ptr<TestLogFormatter> formatter = nullptr)
      : writer_(writer), formatter_(formatter) {}

  folly::StringPiece getType() const override {
    return "test";
  }

  std::shared_ptr<folly::LogHandler> createHandler(const Options& options) override {
    TestWriterFactory writerFactory{writer_};
    if (formatter_ == nullptr) {
      return folly::StandardLogHandlerFactory::createHandler(
          getType(), &writerFactory, options);
    }
    TestFormatterFactory formatterFactory{formatter_};
    return folly::StandardLogHandlerFactory::createHandler(
        getType(), &writerFactory, &formatterFactory, options);
  }

 private:
  std::shared_ptr<TestLogWriter> writer_;
  std::shared_ptr<TestLogFormatter> formatter_;
  class TestWriterFactory : public folly::StandardLogHandlerFactory::WriterFactory {
   public:
    explicit TestWriterFactory(std::shared_ptr<TestLogWriter> writer)
        : writer_(writer) {}

    bool processOption(folly::StringPiece /* name */, folly::StringPiece /* value */)
        override {
      return false;
    }

    std::shared_ptr<folly::LogWriter> createWriter() override {
      return writer_;
    }

   private:
    std::shared_ptr<TestLogWriter> writer_;
  };

  class TestFormatterFactory
      : public folly::StandardLogHandlerFactory::FormatterFactory {
   public:
    explicit TestFormatterFactory(std::shared_ptr<folly::LogFormatter> formatter)
        : formatter_(formatter) {}

    bool processOption(folly::StringPiece /* name */, folly::StringPiece /* value */)
        override {
      return false;
    }

    std::shared_ptr<folly::LogFormatter> createFormatter(
        const std::shared_ptr<folly::LogWriter>& /* logWriter */) override {
      return formatter_;
    }

   private:
    std::shared_ptr<folly::LogFormatter> formatter_;
  };
};
} // namespace
#endif // 0

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

/*
 * An RAII object to be constructed at the beginning of main() and destructed
 * implicitly at the end of main().
 *
 * The constructor performs the same setup as folly::init(), including
 * initializing singletons managed by folly::Singleton.
 *
 * The destructor destroys all singletons managed by folly::Singleton, yielding
 * better shutdown behavior when performed at the end of main(). In particular,
 * this guarantees that all singletons managed by folly::Singleton are destroyed
 * before all Meyers singletons are destroyed.
 */
class Init {
 public:
  // Force ctor & dtor out of line for better stack traces even with LTO.
  FOLLY_NOINLINE Init(int argc, char* argv[], boost::optional<std::string> log_config, bool removeFlags = true);
  FOLLY_NOINLINE ~Init();

  Init(Init const&) = delete;
  Init(Init&&) = delete;
  Init& operator=(Init const&) = delete;
  Init& operator=(Init&&) = delete;
};

Init::Init(int argc, char* argv[],
    boost::optional<std::string> log_config, bool removeFlags) {
#if FOLLY_USE_SYMBOLIZER
  // Install the handler now, to trap errors received during startup.
  // The callbacks, if any, can be installed later
  folly::symbolizer::installFatalSignalHandler();
#elif !defined(_WIN32)
  google::InstallFailureSignalHandler();
#endif

  // Move from the registration phase to the "you can actually instantiate
  // things now" phase.
  folly::SingletonVault::singleton()->registrationComplete();

  // similar to https://github.com/facebook/folly/blob/master/folly/init/Init.cpp#L49
  // but removed ParseCommandLineFlags
  // add support fo file logging,
  // see https://github.com/facebook/folly/blob/master/folly/logging/docs/LogHandlers.md#file-handler-type
  folly::LoggerDB::get().registerHandlerFactory(
      std::make_unique<folly::FileHandlerFactory>());

  if(log_config.is_initialized()) {
    CHECK(!log_config.value().empty())
      << "invalid (empty) log configuration";
    folly::initLoggingOrDie(log_config.value());
  } else {
    /// \see https://github.com/facebook/folly/tree/master/folly/logging/docs
    folly::initLoggingOrDie(
      ".:=INFO:default:x; default=stream:stream=stderr; x=stream:stream=stderr");
  }
  auto programName = argc && argv && argc > 0 ? (argv)[0] : "unknown";
  XLOG(DBG9) << "program name is " << programName;
  google::InitGoogleLogging(programName);

#if FOLLY_USE_SYMBOLIZER
  // Don't use glog's DumpStackTraceAndExit; rely on our signal handler.
  google::InstallFailureFunction(abort);

  // Actually install the callbacks into the handler.
  folly::symbolizer::installFatalSignalCallbacks();
#endif
}

Init::~Init() {
  folly::SingletonVault::singleton()->destroyInstances();
}

namespace {
static bool writeToFile(
    folly::StringPiece contents,
    const std::string& path,
    int flags) {
  int fd = folly::openNoInt(path.data(), flags, 0664);
  if (fd == -1) {
    return false;
  }

  // TODO: file timeout https://github.com/connorlarkin1/react-native/blob/master/third-party/folly-2018.10.22.00/folly/logging/test/AsyncFileWriterTest.cpp#L583
  auto written = folly::writeFull(fd, contents.data(), contents.size());
  if (folly::closeNoInt(fd) != 0) {
    return false;
  }

  return written >= 0 && size_t(written) == contents.size();
}

static std::string randomString(size_t minLen, size_t maxLen,
    folly::StringPiece range = "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") {
  assert(minLen <= maxLen);
  assert(!range.empty());

  std::string result(folly::Random::rand32(minLen, maxLen + 1), '\0');
  for (char& c : result) {
    c = range[folly::Random::rand32(range.size())];
  }
  return result;
}

} // anonymous

static bool writeStringToFile(folly::StringPiece contents, const std::string& path) {
  return writeToFile(contents, path, O_CREAT | O_WRONLY | O_TRUNC);
}

static bool appendStringToFile(folly::StringPiece contents, const std::string& path) {
  return writeToFile(contents, path, O_CREAT | O_WRONLY | O_APPEND);
}

static bool atomicallyWriteFileToDisk(
    folly::StringPiece contents,
    const std::string& absFilename) {
  fs::path tempFilePath;
  auto tempFileGuard = folly::makeGuard([&tempFilePath]() {
    if (!tempFilePath.empty()) {
      std::error_code ec;
      fs::remove(tempFilePath.c_str(), ec);
    }
  });

  try {
    const fs::path filePath(absFilename);
    auto fileDir = filePath.parent_path();
    if (fileDir.empty()) {
      return false;
    }
    auto tempFileName = filePath.filename().string() + ".temp-" +
        randomString(/* minLen */ 10, /* maxLen */ 10);
    tempFilePath = fileDir / tempFileName;

    fs::create_directories(fileDir);

    if (!writeStringToFile(contents, tempFilePath.string())) {
      return false;
    }

    fs::rename(tempFilePath, filePath);
    return true;
  } catch (const fs::filesystem_error& e) {
    // TODO: outcome error details
    return false;
  } catch (const std::system_error& e) {
    // TODO: outcome error details
    return false;
  }
}

/*struct CPUTask : public folly::CPUThreadPoolExecutor::Task {
  // Must be noexcept move constructible so it can be used in MPMCQueue
  explicit CPUTask(
      folly::Func&& f,
      std::chrono::milliseconds expiration,
      folly::Func&& expireCallback)
    : Task(std::move(f), expiration, std::move(expireCallback)),
      poison(false) {}
  CPUTask()
    : Task(nullptr, std::chrono::milliseconds(0), nullptr),
      poison(true) {}
  CPUTask(CPUTask&& o) noexcept : Task(std::move(o)), poison(o.poison) {}
  CPUTask(const CPUTask&) = default;
  CPUTask& operator=(const CPUTask&) = default;
  bool poison;
};*/

static std::shared_ptr<folly::CPUThreadPoolExecutor>
    make_pool_executor(unsigned long concurrency, int queue_size,
    bool throw_if_full, std::string_view pool_name) {
  std::unique_ptr<folly::BlockingQueue<folly::CPUThreadPoolExecutor::CPUTask>> task_queue;
  if (throw_if_full) {
      task_queue = std::make_unique<
          folly::LifoSemMPMCQueue<
              folly::CPUThreadPoolExecutor::CPUTask,
              folly::QueueBehaviorIfFull::THROW>
      >(queue_size);
  } else {
      task_queue = std::make_unique<
          folly::LifoSemMPMCQueue<
              folly::CPUThreadPoolExecutor::CPUTask,
              folly::QueueBehaviorIfFull::BLOCK>
      >(queue_size);
  }
  return std::make_shared<folly::CPUThreadPoolExecutor>(
      std::make_pair(concurrency, 1),
      std::move(task_queue),
      std::make_shared<folly::NamedThreadFactory>(pool_name.data()));
}

static std::string readFile(const std::string& in_path) {
  folly::IOBufQueue buf;
  try {
    auto cleanup = gsl::finally([prevPath = fs::current_path()] {
        fs::current_path(prevPath);
    });
    fs::current_path(fs::absolute(srcdir_abs_path));
    const fs::path in_abs_path = fs::absolute(in_path);
    XLOG(DBG9) << "started reading file " << in_abs_path;
    auto in_file = std::make_unique<folly::File>(in_abs_path);
    while (in_file) {
      auto data = buf.preallocate(4000, 4000);
      auto rc = folly::readNoInt(in_file->fd(), data.first, data.second);
      if (rc < 0) {
        XLOG(ERR) << "Read error=" << rc;
        in_file.reset();
        //break;
        std::terminate(); /// TODO: gracefull_shutdown
      } else if (rc == 0) {
        // done
        in_file.reset();
        XLOG(DBG9) << "Read EOF for " << in_abs_path;
        break;
      } else {
        buf.postallocate(rc);
      }
    }
  } catch (const std::system_error& ex) {
    XLOG(ERR) << "ERROR: Could not open file " << in_path
      << " exception = " << folly::exceptionStr(ex);
    //DCHECK(CPU_executor) << "invalid CPU_executor";
    //std::terminate(); // TODO: gracefull_shutdown
    //CPU_executor->stop();
    //return;
    std::terminate(); /// TODO: gracefull_shutdown
  }

  if(buf.empty() || buf.front()->empty()) {
    XLOG(WARNING) << "WARNING: empty input from file " << in_path;
    return "";
  }

  auto queueToString = [](const folly::IOBufQueue& queue) {
    std::string out;
    queue.appendToString(out);
    return out;
  };

  return queueToString(buf);
}

static void processTemplate(const std::string& in_path,
    const std::string out_path) {
  using namespace ::CXTPL::core::errors;

  const std::string file_contents = readFile(in_path);
  XLOG(DBG9) << "input file contents" << file_contents;

  CXTPL::core::Generator template_engine;

  base::string16 clean_contents;
  CXTPL::core::defaults::ConvertResponseToUTF16(
    /* unknown encoding */ "",
    file_contents,
    &clean_contents);

  const outcome::result<std::string, GeneratorErrorExtraInfo>
    genResult
      = template_engine.generate_from_UTF16(
        clean_contents);

  if(genResult.has_error()) {
    /*if(genResult.error().ec == GeneratorError::EMPTY_INPUT) {
      ///\note assume not error, just empty file
      XLOG(WARNING) << "WARNING: empty string as Generator input: "
        << genResult.error().extra_info;
      //return;
      std::terminate(); /// TODO: gracefull_shutdown
    } else */{
      XLOG(ERR) << "=== ERROR START ===";
      XLOG(ERR) << "ERROR message: " <<
        make_error_code(genResult.error().ec).message();
      XLOG(ERR) << "ERROR category: " <<
        " " << make_error_code(genResult.error().ec).category().name();
      XLOG(ERR) << "ERROR info: " <<
        " " << genResult.error().extra_info;
      XLOG(ERR) << "input data: "
        /// \note limit to first 200 symbols
        << file_contents.substr(0, std::min(200UL, file_contents.size()))
        << "...";
      // TODO: file path here
      XLOG(ERR) << "=== ERROR END ===";
      //std::terminate(); // TODO: gracefull_shutdown
      //CPU_executor->stop();
      DCHECK(CPU_executor) << "invalid CPU_executor";
      //return;
      std::terminate(); /// TODO: gracefull_shutdown
    }
  }

  if(!genResult.has_value() || genResult.value().empty()) {
    XLOG(WARNING) << "WARNING: empty output from file " << in_path;
    return;
  }

  XLOG(DBG9) << "output file contents" << genResult.value();

  XLOG(DBG9) << "input path for generator: " << in_path;
  XLOG(DBG9) << "generated output data: " << genResult.value();


  if(generator_path_arg.is_initialized()
     && !generator_path_arg.value().empty())
  {
#if defined(CLING_IS_ON)
    DCHECK(g_interp);

    const std::string out_abs_path_str = fs::absolute(out_path).u8string();

    {
      std::ostringstream sstr;
      // scope begin
      sstr << "[](){";
      // vars begin
      sstr << "const std::string& cling_genResult = ";
      sstr << "*(const std::string*)("
              // Pass a pointer into cling as a string.
           << std::hex << std::showbase
           << reinterpret_cast<size_t>(&genResult.value()) << ");";
      sstr << "const std::string& out_abs_path_str = ";
      sstr << "*(const std::string*)("
              // Pass a pointer into cling as a string.
           << std::hex << std::showbase
           << reinterpret_cast<size_t>(&out_abs_path_str) << ");";

      // vars end
      sstr << "return ";
      sstr << "onBeforeTemplateGeneration(";
      sstr << "cling_genResult, out_abs_path_str";
      sstr << ");";
      // scope end
      sstr << "}();";
      g_interp->processCode(sstr.str());
    }

    /// \note we need to isolate interpreted code to prevent
    /// variable declaration in parent-scope, so
    /// wrap everything into lambda
    {
      std::ostringstream sstr;
      // scope begin
      sstr << "[](){";
      /// \note cxtpl_output is predefined variable name
      //sstr << "std::string cxtpl_output;"; /// \todo get `cxtpl_output` as cling::Value
      sstr << "{";
      sstr << genResult.value();
      sstr << "}";
      //sstr << "return cxtpl_output;"; /// \todo get `cxtpl_output` as cling::Value
      // scope end
      sstr << "}();";
      g_interp->processCode(sstr.str());
    }

    {
      std::ostringstream sstr;
      // scope begin
      sstr << "[](){";
      // vars begin
      sstr << "const std::string& cling_genResult = ";
      sstr << "*(const std::string*)("
              // Pass a pointer into cling as a string.
           << std::hex << std::showbase
           << reinterpret_cast<size_t>(&genResult.value()) << ");";
      sstr << "const std::string& out_abs_path_str = ";
      sstr << "*(const std::string*)("
              // Pass a pointer into cling as a string.
           << std::hex << std::showbase
           << reinterpret_cast<size_t>(&out_abs_path_str) << ");";

      // vars end
      sstr << "return ";
      sstr << "onAfterTemplateGeneration(";
      sstr << "cling_genResult, out_abs_path_str";
      sstr << ");";
      // scope end
      sstr << "}();";
      g_interp->processCode(sstr.str());
    }
#else
    CHECK(false) << "CLING must be enabled to use custom generators";
#endif // CLING_IS_ON
  } else {
    // see folly/io/async/AsyncPipe.cpp#L223
    try {
      auto cleanup = gsl::finally([prevPath = fs::current_path()] {
          fs::current_path(prevPath);
      });
      fs::current_path(fs::absolute(resdir_abs_path));

      const fs::path out_abs_path = fs::absolute(out_path);
      XLOG(DBG9) << "started writing into file " << out_abs_path;
      if(!atomicallyWriteFileToDisk(genResult.value(), out_abs_path)) {
        XLOG(ERR) << "ERROR: can`t write to file " << out_abs_path;
        //std::terminate(); // TODO: gracefull_shutdown
        //CPU_executor->stop();
        DCHECK(CPU_executor) << "invalid CPU_executor";
        //return;
        std::terminate(); /// TODO: gracefull_shutdown
      }
      XLOG(DBG6) << "Wrote " << genResult.value().size() << " bytes to file " << out_abs_path;
    } catch (const std::system_error& ex) {
      XLOG(ERR) << "ERROR: Could not open file " << in_path
        << " exception = " << folly::exceptionStr(ex);
      //std::terminate(); // TODO: gracefull_shutdown
      //CPU_executor->stop();
      DCHECK(CPU_executor) << "invalid CPU_executor";
      //return;
      std::terminate(); /// TODO: gracefull_shutdown
    }
  }
}

static void run_generation() {
  XLOG(DBG9) << "Current path is " << fs::current_path();
  XLOG(DBG9) << "srcdir: Local path is " << srcdir_arg.get_value_or("");
  XLOG(DBG9) << "srcdir: Absolute path is " << srcdir_abs_path;
  XLOG(DBG9) << "resdir: Local path is " << resdir_arg.get_value_or("");
  XLOG(DBG9) << "resdir: Absolute path is " << resdir_abs_path;

  DCHECK(!CPU_executor);

  CPU_executor = make_pool_executor(threads_arg, 1024,
    /*throw_if_full*/ false, /*pool_name*/ "CXTPL_tool_file_queue");
  DCHECK(CPU_executor) << "invalid CPU_executor";
  DCHECK(std::equal_to<size_t>{}(CPU_executor->numThreads(), threads_arg));

  CPU_executor->subscribeToTaskStats(
    [&](folly::ThreadPoolExecutor::TaskStats stats) {
        XLOG(DBG9) << "Done task";
        XLOG(DBG9) << "task.stats.expired = " << stats.expired;
        {
          long int diff_ms
            = std::chrono::duration_cast<std::chrono::milliseconds>(
                         stats.runTime)
                         .count();
          XLOG(DBG9) << "task.stats.runTime = " << diff_ms
            << " (milliseconds) or " << stats.runTime.count() << " (nanoseconds)";
          if(single_task_timeout.is_initialized() && stats.runTime > single_task_timeout.value()) {
            XLOG(WARNING) << "task.stats.runTime timed out "
                          "(single task timeout is "
                          << single_task_timeout.value().count() << ")";
          }
        }
        {
          long int diff_ms
            = std::chrono::duration_cast<std::chrono::milliseconds>(
                         stats.waitTime)
                         .count();
          XLOG(DBG9) << "task.stats.waitTime = " << diff_ms
            << " (milliseconds) or " << stats.waitTime.count() << " (nanoseconds)";
        }
    }
  );

  /// \note Idle time before ThreadPoolExecutor threads are joined
  /// may happen if we will try to add too many tasks
  /// (before execution of tasks)
  CPU_executor->setThreadDeathTimeout(std::chrono::seconds(60));

  long int in_index = 0;
  for(auto it_path = in_args.begin();
      it_path != in_args.end(); (it_path++, in_index++)) {
    DCHECK(in_index < in_args.size());
    DCHECK(in_index < out_args.size());

    auto tout = single_task_timeout.is_initialized()
      ? single_task_timeout.value() * in_index
      //\note std::chrono::milliseconds::max() not supported here
      : std::chrono::milliseconds{1000*60*60*24*7};

    if(global_timeout.is_initialized() && global_timeout.value() < tout) {
      tout = global_timeout.value();
    }


    //typedef folly::UndelayedDestruction<folly::HHWheelTimerHighRes> StackWheelTimerUs;

    XLOG(DBG4) << "Added task for " << *it_path;

    CPU_executor->add(
       ///\note copied data for lambda
      [/*global_timeout,*/ in_index, in_path = *it_path, out_path = out_args.at(in_index)] {

        processTemplate(std::move(in_path), std::move(out_path));

        // TODO: timeouts
        //std::this_thread::sleep_for(std::chrono::milliseconds{58});

      },
      folly::Executor::MID_PRI,
      tout, // copyed
        ///\note copied data for lambda
        [in_path = *it_path, out_path = out_args.at(in_index),
         tout /*copyed, timeout may change*/]()
        {
          XLOG(ERR) << "All tasks for file generation timed out after task"
            << " (" << in_path << " -> "  << out_path << ") "
            << "timed out in : " << tout.count() << " milliseconds";
          std::terminate(); /// TODO: gracefull_shutdown
          //CPU_executor->stop();
        }
      );

    ///\note original vector elements must be valid
    DCHECK(!in_args.at(in_index).empty());
    DCHECK(!out_args.at(in_index).empty());
  }

  CPU_executor->join();

  /// \note don`t use same executor again
  CPU_executor.reset();
}

static bool startsWith(const std::string& in, const std::string& prefix) {
  return !in.compare(0, prefix.size(), prefix);
};

static std::string removePrefix(const std::string& from,
    const std::string& prefix) {
  return from.substr( prefix.size(), from.size() );
};

int main(int argc, char* argv[]) {
  try {
    const char* help_arg_name = "help";
    const char* in_arg_name = "input_files,I";
    const char* out_arg_name = "output_files,O";
    const char* srcdir_arg_name = "srcdir,S";
    const char* resdir_arg_name = "resdir,R";
    const char* version_arg_name = "version,V";
    const char* threads_arg_name = "threads,J";
    const char* log_arg_name = "log,L";
    const char* global_timeout_arg_name = "global_timeout_ms,G";
    const char* single_task_timeout_arg_name = "single_task_timeout_ms,T";
    const char* generator_path_arg_name = "generator_path";

    // TODO: --watch arg https://github.com/blockspacer/skia-opengl-emscripten/blob/bb16ab108bc4018890f4ff3179250b76c0d9053b/src/chromium/net/dns/dns_config_service_posix.cc#L279

    po::options_description desc("Allowed options");

    int single_task_timeout_arg;
    int global_timeout_arg;

    desc.add_options()
      (help_arg_name, "produce help message")
      (version_arg_name, "produce version message")
      (threads_arg_name, po::value(&threads_arg)->default_value(2), "number of threads")
      (log_arg_name, po::value(&log_config)->default_value(boost::none, ""), "log configuration")
      (global_timeout_arg_name, po::value<int>(&global_timeout_arg)->default_value(0), "global timeout")
      (single_task_timeout_arg_name, po::value<int>(&single_task_timeout_arg)->default_value(0), "single task timeout")
      (resdir_arg_name, po::value(&resdir_arg)->default_value(boost::none, ""), "change output directory path (where to place generated files)")
      (srcdir_arg_name, po::value(&srcdir_arg)->default_value(boost::none, ""), "change current working directory path (path to template files)")
      (in_arg_name, po::value(&in_args)->multitoken(), "list of template files that must be used for C++ code generation")
      (generator_path_arg_name, po::value(&generator_path_arg)->default_value(boost::none, ""), "path to generator C++ file")
      // TODO: outfile_pattern_name
      //(outfile_pattern_name, po::value(&outfile_pattern)->default_value("{out.dir}{in.filename}{in.ext}.cpp"), "output format")
      (out_arg_name, po::value(&out_args)->multitoken(), "list of C++ files that must be generated from templates");

    po::variables_map vm;
    auto parsed_options = po::command_line_parser(argc, argv).
      options(desc).allow_unregistered().run();
    po::store(parsed_options, vm);

    po::notify(vm);

    if(single_task_timeout_arg != 0) {
      single_task_timeout =
        std::chrono::milliseconds{single_task_timeout_arg};
    }

    if(global_timeout_arg != 0) {
      global_timeout =
        std::chrono::milliseconds{global_timeout_arg};
    }

    if(generator_path_arg.is_initialized()
       && !generator_path_arg.value().empty()) {
      /// \todo make Cling thread-safe
      XLOG(WARNING) << "Disabled multithreading in CXTPL_tool due "
                       "to argument: generator_path";
      threads_arg = 1;
    }

    /// \note no logging before Init!
    Init(argc, argv, log_config);
    XLOG(DBG9) << "Initialized CXTPL_tool";

#if defined(CLING_IS_ON)
    /// \note forward unregistered options to clang libtooling
    for (const auto& o : parsed_options.options) {
        if (vm.find(o.string_key) == vm.end()) {
            // an unknown option
            for(size_t i = 0; i < o.value.size(); i++) {
                const std::string combined_for_clang
                  = o.string_key + o.value[i];
                XLOG(DBG9) << "forwarded unregistered option "
                              "to cling: "
                              << combined_for_clang;
                if(combined_for_clang.empty()) {
                  continue;
                }
                if(startsWith(combined_for_clang, "-extra-arg=")) {
                  const std::string combined_for_cling
                    = removePrefix(combined_for_clang, "-extra-arg=");
                  if(combined_for_cling.empty()) {
                    continue;
                  }
                  cling_utils::InterpreterModule::
                    extra_args.push_back(combined_for_cling);
                }
            }
        }
    }
#endif // CLING_IS_ON

    if (vm.count(help_arg_name)) {
      XLOG(INFO) << desc;
      return EXIT_SUCCESS;
    }

    if (vm.count("version") || vm.count("V")) {
      XLOG(INFO) << CXTPL_tool_VERSION;
      return EXIT_SUCCESS;
    }

    if(generator_path_arg.is_initialized()
       && !generator_path_arg.value().empty()) {
  #if defined(CLING_IS_ON)
      generator_file_contents = readFile(generator_path_arg.value_or(""));
      XLOG(DBG9) << "generator file contents" << generator_file_contents;
      DCHECK(!g_interp);
      g_interp
        = std::make_unique<cling_utils::InterpreterModule>(
            "CXTPL_Cling_Interpreter");
      if(!generator_file_contents.empty()) {
        g_interp->processCode(generator_file_contents.c_str());
      } else {
        DCHECK(false) << "empty file for custom generator";
      }
  #else
      CHECK(false) << "CLING must be enabled to use custom generators";
  #endif // CLING_IS_ON
    }

    if (in_args.empty()) {
      XLOG(ERR) << "ERROR: no input files.";
      return EXIT_FAILURE;
    }

    if (out_args.empty()) {
      XLOG(ERR) << "ERROR: no output files.";
      return EXIT_FAILURE;
    }

    auto fix_inputs = [](std::vector<std::string>& inout) {
      std::vector<std::string> v;
      for(const auto& it: inout) {
        XLOG(DBG9) << "before: " << it;
        // split a string by blank spaces unless it is in quotes
        std::istringstream iss(it);
          std::string s;
          while (iss >> std::quoted(s)) {
              if(!s.empty()) {
                v.push_back(s);
                XLOG(DBG9) << "after: " << s;
              }
          }
      }
      inout = v;
    };

    fix_inputs(in_args);

    if (out_args.empty()) {
      size_t i;
      for(const auto& it: in_args) {
        out_args.push_back(in_args.at(i) + ".generated.cpp");
        i++;
      }
    }

    fix_inputs(out_args);

    XLOG(DBG9) << "inputs (" << in_args.size() << "): ";
    for(const auto& it: in_args) {
      XLOG(DBG9) << " + " << it;
    }

    XLOG(DBG9) << "outputs (" << out_args.size() << ") ";
    for(const auto& it: out_args) {
      XLOG(DBG9) << " + " << it;
    }

    if (in_args.size() != out_args.size()) {
      XLOG(INFO) << "inputs (" << in_args.size() << "): ";
      for(const auto& it: in_args) {
        XLOG(INFO) << " + " << it;
      }

      XLOG(INFO) << "outputs (" << out_args.size() << ") ";
      for(const auto& it: out_args) {
        XLOG(INFO) << " + " << it;
      }
      XLOG(ERR) << "ERROR: number of input files "
                   "must be same as the number of output files.";
      return EXIT_FAILURE;
    }
  }
  catch(std::exception& e) {
    XLOG(ERR) << "ERROR: " << e.what();
    return EXIT_FAILURE;
  }
  catch(...) {
    XLOG(ERR) << "ERROR: Exception of unknown type!";
    return EXIT_FAILURE;
  }

#ifdef HAS_ICU
  XLOG(DBG9) << "called base::i18n::InitializeICU";
  bool icu_initialized = base::i18n::InitializeICU();
#if defined(OS_EMSCRIPTEN)
  if(!icu_initialized) {
    DCHECK(false);
  }
#endif // OS_EMSCRIPTEN
#endif // HAS_ICU

  srcdir_abs_path = fs::absolute(fs::current_path());
  if(srcdir_arg.is_initialized() && !srcdir_arg.value().empty()) {
    srcdir_abs_path = fs::absolute(fs::path(srcdir_arg.value()));
    if(!fs::is_directory(srcdir_abs_path)) {
      XLOG(ERR) << srcdir_arg.value() << " must be directory " << srcdir_abs_path;
      return EXIT_FAILURE;
    }
    fs::current_path(srcdir_arg.value());
  }

  resdir_abs_path = fs::absolute(fs::current_path());
  if(resdir_arg.is_initialized() && !resdir_arg.value().empty()) {
    resdir_abs_path = fs::absolute(fs::path(resdir_arg.value()));
    if(!fs::is_directory(resdir_abs_path)) {
      XLOG(WARNING) << "created directory " << resdir_abs_path;
      fs::create_directories(resdir_abs_path);
    }
  }

  auto chrono_then = std::chrono::steady_clock::now();

  run_generation();

  long int diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();
  long int diff_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();
  XLOG(DBG9) << "Done in : " << diff_ms << " milliseconds (" << diff_ns << " nanoseconds)";
  return EXIT_SUCCESS;
}
