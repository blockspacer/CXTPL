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

#include <folly/io/async/ScopedEventBaseThread.h>
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
#include <folly/experimental/TimerFDTimeoutManager.h>
#include <folly/experimental/STTimerFDTimeoutManager.h>
#include <folly/io/async/test/Util.h>
//#include <folly/Benchmark.h>
#include <folly/experimental/STTimerFDTimeoutManager.h>
#include <folly/experimental/TimerFDTimeoutManager.h>
#include <folly/io/async/test/UndelayedDestruction.h>
#include <folly/executors/TimedDrivableExecutor.h>

#if FOLLY_USE_SYMBOLIZER
#include <folly/experimental/symbolizer/SignalHandler.h> // @manual
#endif
#include <folly/portability/GFlags.h>

// boost log or
// #include <glog/logging.h>
#include <glog/logging.h>

#include "core/errors/errors.hpp"
#include "core/CXTPL.hpp"

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

static unsigned long threads_arg;

static fs::path srcdir_abs_path;

static fs::path resdir_abs_path;

static boost::optional<std::chrono::milliseconds> single_task_timeout;
//    = std::chrono::milliseconds(100);

  /// \note summary timeout for all files in one CPU_executor
static boost::optional<std::chrono::milliseconds> global_timeout;
//= /*
//    (CPU_executor->getTaskQueueSize() + 1) **/
//    std::chrono::seconds{3600};

#if 0
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

/// \see https://github.com/facebook/folly/tree/master/folly/logging/docs
/*FOLLY_INIT_LOGGING_CONFIG(
//      ".:=INFO:default; default=stream:stream=stderr,async=false"
//    ".:=INFO:default; default=stream:stream=stderr,async=false");
//    ".=INFO:default; default=null");
//    ".=INFO,folly=INFO; default:async=true,sync_level=INFO"
//    ".=DBG9"
      ".:=ERROR:default:x; default=stream:stream=stderr; x=stream:stream=stderr"
);*/

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

/*template<class T>
std::ostream& operator<<(std::ostream& os, const unsigned int& v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}*/

/*template<class T>
std::ostream& operator<<(std::ostream& os, std::optional<std::string>& v)
{
    v.emplace("");
    //copy(v.value().begin(), v.value().end(), std::ostream_iterator<T>(os, " "));
    return os;
}*/

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

#include <folly/Conv.h>
#include <folly/portability/GFlags.h>
#include <folly/ssl/Init.h>
#include <glog/logging.h>

Init::Init(int argc, char* argv[],
    boost::optional<std::string> log_config, bool removeFlags) {
#if FOLLY_USE_SYMBOLIZER
  // Install the handler now, to trap errors received during startup.
  // The callbacks, if any, can be installed later
  folly::symbolizer::installFatalSignalHandler();
#elif !defined(_WIN32)
  google::InstallFailureSignalHandler();
#endif

  // folly::Init() will automatically initialize the logging settings based on
  // the FOLLY_INIT_LOGGING_CONFIG declaration above and the --logging command
  // line flag.
  //folly::Init init(&argc, &argv, /*removeFlags*/ true);

  // Move from the registration phase to the "you can actually instantiate
  // things now" phase.
  folly::SingletonVault::singleton()->registrationComplete();

  // similar to https://github.com/facebook/folly/blob/master/folly/init/Init.cpp#L49
  // but removed ParseCommandLineFlags
  /*if(FLAGS_v) {
    /// \note can`t use LOG here
    std::cout << "LoggingConfig: " << folly::getBaseLoggingConfig() << std::endl;
  }*/
  std::cout << "log_config.value(): " << log_config.value() << std::endl;

  // add support fo file logging,
  // see https://github.com/facebook/folly/blob/master/folly/logging/docs/LogHandlers.md#file-handler-type
  folly::LoggerDB::get().registerHandlerFactory(
      std::make_unique<folly::FileHandlerFactory>());

  if(log_config.is_initialized()) {
    CHECK(!log_config.value().empty())
      << "invalid (empty) log configuration";
    folly::initLoggingOrDie(log_config.value());
  } else {
    folly::initLoggingOrDie(
      ".:=ERROR:default:x; default=stream:stream=stderr; x=stream:stream=stderr");
      //folly::getBaseLoggingConfig());
  }
  auto programName = argc && argv && argc > 0 ? (argv)[0] : "unknown";
  XLOG(DBG4) << "programName " << programName << std::endl;
  google::InitGoogleLogging(programName);

  //gflags::SetCommandLineOptionWithMode(
  //    "minloglevel", "0", gflags::SET_FLAGS_DEFAULT);

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
bool writeToFile(
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

std::string randomString(size_t minLen, size_t maxLen,
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

bool writeStringToFile(folly::StringPiece contents, const std::string& path) {
  return writeToFile(contents, path, O_CREAT | O_WRONLY | O_TRUNC);
}

bool appendStringToFile(folly::StringPiece contents, const std::string& path) {
  return writeToFile(contents, path, O_CREAT | O_WRONLY | O_APPEND);
}

bool atomicallyWriteFileToDisk(
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

std::shared_ptr<folly::CPUThreadPoolExecutor>
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

void processTemplate(const std::string& in_path, const std::string out_path) {
  using namespace ::CXTPL::core::errors;

  folly::IOBufQueue buf;
  try {
    const fs::path in_abs_path = fs::absolute(in_path, srcdir_abs_path);
    XLOG(DBG9) << "started reading file " << in_abs_path;
    auto in_file = std::make_unique<folly::File>(in_abs_path);
    while (in_file) {
      auto data = buf.preallocate(4000, 4000);
      auto rc = folly::readNoInt(in_file->fd(), data.first, data.second);
      if (rc < 0) {
        XLOG(DBG4) << "Read error=" << rc;
        in_file.reset();
        break;
      } else if (rc == 0) {
        // done
        in_file.reset();
        XLOG(DBG4) << "Read EOF";
        break;
      } else {
        buf.postallocate(rc);
      }
    }
  } catch (const std::system_error& ex) {
    DCHECK(CPU_executor) << "invalid CPU_executor";
    CPU_executor->addWithPriority(
      [p = std::move(in_path), e = std::move(ex)](){
        XLOG(ERR) << "ERROR: Could not open file " << p
          << " exception = " << folly::exceptionStr(e);
        //std::terminate(); // TODO: gracefull_shutdown
        //CPU_executor->stop();
    }, folly::Executor::HI_PRI);
    return;
  }

  if(buf.empty() || buf.front()->empty()) {
    XLOG(DBG4) << "WARNING: empty input from file " << in_path;
    //continue;
    return;
  }

  auto queueToString = [](const folly::IOBufQueue& queue) {
    std::string out;
    queue.appendToString(out);
    return out;
  };

  const std::string input = queueToString(buf);

  CXTPL::core::Generator template_engine;

  const outcome::result<std::string, GeneratorErrorExtraInfo> genResult
    = template_engine.generate(input.c_str());

  if(genResult.has_error()) {
    if(genResult.error().ec == GeneratorError::EMPTY_INPUT) {
      ///\note assume not error, just empty file
      XLOG(WARNING) << "WARNING: empty string as Generator input";
      return;
    } else {
      DCHECK(CPU_executor) << "invalid CPU_executor";
      CPU_executor->addWithPriority(
        [r = std::move(genResult), i = std::move(input)](){
          XLOG(ERR) << "=== ERROR START ===";
          XLOG(ERR) << "ERROR message: " <<
            make_error_code(r.error().ec).message();
          XLOG(ERR) << "ERROR category: " <<
            " " << make_error_code(r.error().ec).category().name();
          XLOG(ERR) << "ERROR info: " <<
            " " << r.error().extra_info;
          XLOG(ERR) << "input data: " << i;
          // TODO: file path here
          XLOG(ERR) << "=== ERROR END ===";
          //std::terminate(); // TODO: gracefull_shutdown
          //CPU_executor->stop();
      }, folly::Executor::HI_PRI);
      return;
    }
  }

  if(!genResult.has_value() || genResult.value().empty()) {
    XLOG(DBG4) << "WARNING: empty output from file " << in_path;
    return;
  }

  XLOG(DBG9) << "input path for generator: " << in_path;
  XLOG(DBG9) << "generated output data: " << genResult.value();

  // see folly/io/async/AsyncPipe.cpp#L223
  try {
    const fs::path out_abs_path = fs::absolute(out_path, resdir_abs_path);
    /*auto in_file = std::make_unique<folly::File>(out_path);
    size_t bytesToWrite = genResult.value().size();
    auto ret = folly::writeFull(in_file->fd(),
      genResult.value().data(), bytesToWrite);
    if (ret != -1 || errno != EAGAIN) {
      folly::checkUnixError(ret, "write");
      XLOG(DBG6) << "Wrote " << ret << " lines to file " << out_path;
    }*/
    XLOG(DBG9) << "started writing into file " << out_abs_path;
    if(!atomicallyWriteFileToDisk(genResult.value(), out_abs_path)) {
      DCHECK(CPU_executor) << "invalid CPU_executor";
      CPU_executor->addWithPriority(
        [p = std::move(out_abs_path)](){
          XLOG(ERR) << "ERROR: can`t write to file " << p;
          //std::terminate(); // TODO: gracefull_shutdown
          //CPU_executor->stop();
      }, folly::Executor::HI_PRI);
      return;
    }
    XLOG(DBG6) << "Wrote " << genResult.value().size() << " bytes to file " << out_abs_path;
  } catch (const std::system_error& ex) {
    DCHECK(CPU_executor) << "invalid CPU_executor";
    CPU_executor->addWithPriority(
      [p = std::move(in_path), e = std::move(ex)](){
        XLOG(ERR) << "ERROR: Could not open file " << p
          << " exception = " << folly::exceptionStr(e);
        //std::terminate(); // TODO: gracefull_shutdown
        //CPU_executor->stop();
    }, folly::Executor::HI_PRI);
    return;
  }
}

void run_generation() {
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
        XLOG(DBG4) << "Done task";
        XLOG(DBG4) << "task.stats.expired = " << stats.expired;
        {
          long int diff_ms
            = std::chrono::duration_cast<std::chrono::milliseconds>(
                         stats.runTime)
                         .count();
          XLOG(DBG4) << "task.stats.runTime = " << diff_ms
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
          XLOG(DBG4) << "task.stats.waitTime = " << diff_ms
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

    //XLOG(DBG4) << "tout2 = " << tout.count();

    if(global_timeout.is_initialized() && global_timeout.value() < tout) {
      tout = global_timeout.value();
    }

    //tout = std::chrono::milliseconds{500};

    //XLOG(DBG4) << "tout1 = " << tout.count();

    typedef folly::UndelayedDestruction<folly::HHWheelTimerHighRes> StackWheelTimerUs;

    XLOG(DBG4) << "Added task for " << *it_path;

    CPU_executor->add(
       ///\note copied data for lambda
      [/*global_timeout,*/ in_index, in_path = *it_path, out_path = out_args.at(in_index)] {

        processTemplate(std::move(in_path), std::move(out_path));

        std::this_thread::sleep_for(std::chrono::milliseconds{58});
#if 0
        folly::EventBase evb;
        folly::STTimerFDTimeoutManager timeoutMgr(&evb);
        std::unique_ptr<folly::AsyncTimeout> ts
          = folly::AsyncTimeout::make(evb,[in_path, out_path]()
        {
          processTemplate(std::move(in_path), std::move(out_path));
        });
        //timeoutMgr.scheduleTimeout(ts.get(),
        //    std::chrono::microseconds{1});
        StackWheelTimerUs t(&timeoutMgr,
          std::chrono::microseconds(single_task_timeout));
        t.scheduleTimeout([](){
        },
            std::chrono::microseconds{1});
        //evb.loop();

        folly::TimedDrivableExecutor exec;
        auto f1 = [&]() {
          // TODO >>>>>>>>>
          std::this_thread::sleep_for(std::chrono::milliseconds{300});
        };
        exec.add(f1);
        exec.try_drive_until(
          std::chrono::system_clock::now() + std::chrono::milliseconds(100));

        folly::Baton<> barrier;

        folly::EventBase evb;
        evb.runInEventBaseThread([&evb, &barrier](){
          XLOG(INFO) << "INFO1";
          barrier.post();
          //std::this_thread::sleep_for(std::chrono::milliseconds{300});
          while(true){}
          XLOG(INFO) << "INFO2";
          XLOG(INFO) << "INFO3";
        });
        XLOG(INFO) << "loopOnce1";
        //evb.loopOnce();
        XLOG(INFO) << "loopOnce2";

        if (!barrier.try_wait_for(single_task_timeout)) {
            XLOG(ERR) << "Timeout while running task"
              << " (" << in_path << " -> "  << out_path << ") ";
            //CPU_executor->stop();
            evb.terminateLoopSoon();
        }
        barrier.reset();
        XLOG(INFO) << "loopOnce3";
#endif

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
          //std::terminate(); // TODO: gracefull_shutdown
          //CPU_executor->stop();
        }
      );

    ///\note original vector elements must be valid
    DCHECK(!in_args.at(in_index).empty());
    DCHECK(!out_args.at(in_index).empty());
  }

  //XLOG(INFO) << "CPU_executorCPU_executor";

  CPU_executor->join();
  //CPU_executor->stop();

#if 0
    std::chrono::milliseconds timeout = /*
      (CPU_executor->getTaskQueueSize() + 1) **/
      std::chrono::milliseconds{99999};
    CPU_executor->add(
       ///\note copied data for lambda
      [timeout, in_index] {
        //processTemplate(std::move(in_path), std::move(out_path));
        XLOG(DBG4) << "asddddddddddd : ";
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
      },
      folly::Executor::MID_PRI,
      timeout, // copyed
        ///\note copied data for lambda
        [timeout](){
          XLOG(ERR) << "Task for file generation "
            << " (" << " -> " << ") "
            << "timed out in : " << timeout.count() << " milliseconds";
          //std::terminate();
        }
      );
    CPU_executor->add(
       ///\note copied data for lambda
      [timeout, in_index] {
        //processTemplate(std::move(in_path), std::move(out_path));
        XLOG(DBG4) << "asddddddddddd : ";
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
      },
      folly::Executor::MID_PRI,
      timeout, // copyed
        ///\note copied data for lambda
        [timeout](){
          XLOG(ERR) << "Task for file generation "
            << " (" << " -> " << ") "
            << "timed out in : " << timeout.count() << " milliseconds";
          //std::terminate();
        }
      );
      CPU_executor->join();
#endif

  /// \note don`t use same executor again
  CPU_executor.reset();
}

int main(int argc, char* argv[]) {
  try {
    const char* help_arg_name = "help";
    const char* in_arg_name = "input_files,I";
    const char* out_arg_name = "output_files,O";
    const char* srcdir_arg_name = "srcdir,S";
    const char* resdir_arg_name = "resdir,R";
    const char* threads_arg_name = "threads,J";
    const char* log_arg_name = "log,L";
    const char* global_timeout_arg_name = "global_timeout_ms,G";
    const char* single_task_timeout_arg_name = "single_task_timeout_ms,T";
    /*const char* log_verbosity_name = "logverbosity,V";
    const char* minloglevel_name = "minloglevel,L";*/

    po::options_description desc("Allowed options");

    int single_task_timeout_arg;
    int global_timeout_arg;

    desc.add_options()
      (help_arg_name, "produce help message")
      (threads_arg_name, po::value(&threads_arg)->default_value(2), "number of threads")
      (log_arg_name, po::value(&log_config)->default_value(boost::none, ""), "log configuration")
      (global_timeout_arg_name, po::value<int>(&global_timeout_arg)->default_value(0), "log configuration")
      (single_task_timeout_arg_name, po::value<int>(&single_task_timeout_arg)->default_value(0), "log configuration")
      //(log_verbosity_name, po::value(&log_verbosity)->default_value(9), "log verbosity")
      //(minloglevel_name, po::value(&minloglevel)->default_value(0), "log verbosity")
      (resdir_arg_name, po::value(&resdir_arg)->default_value(boost::none, ""), "change current working directory path")
      (srcdir_arg_name, po::value(&srcdir_arg)->default_value(boost::none, ""), "change current working directory path")
      (in_arg_name, po::value(&in_args)->multitoken(), "template files")
      (out_arg_name, po::value(&out_args)->multitoken(), "where to place C++ code files generated from template");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(single_task_timeout_arg != 0) {
      std::cout << "single_task_timeout_arg "
        << single_task_timeout_arg <<std::endl;
      single_task_timeout =
        std::chrono::milliseconds{single_task_timeout_arg};
    }

    if(global_timeout_arg != 0) {
      std::cout << "global_timeout_arg "
        << global_timeout_arg <<std::endl;
      global_timeout =
        std::chrono::milliseconds{global_timeout_arg};
    }

    //FLAGS_alsologtostderr = true;
    //FLAGS_minloglevel = minloglevel;
    //FLAGS_v = log_verbosity;
    Init(argc, argv, log_config);

    /*auto& db = folly::LoggerDB::get();
    folly::LogCategory* category = db.getCategoryOrNull("");
    CHECK(category) << "unknown category";
    if(category) {
      folly::Logger(category).getCategory()
        ->setLevel(folly::LogLevel::DBG9);
        //folly::stringToLogLevel("ERROR"), true);
    }*/

    /*if (!srcdir_arg.is_initialized()) {
      XLOG(ERR) << "ERROR: no srcdir_arg.";
      return EXIT_SUCCESS;
    }*/
    //XLOG(DBG9) << "desc";

    if (vm.count(help_arg_name)) {
      XLOG(INFO) << desc;
      return EXIT_SUCCESS;
    }

    if (in_args.empty()) {
      XLOG(ERR) << "ERROR: no input files.";
      return EXIT_FAILURE;
    }

    if (out_args.empty()) {
      XLOG(ERR) << "ERROR: no output files.";
      return EXIT_FAILURE;
    }

    if (out_args.empty()) {
      size_t i;
      for(const auto& it: in_args) {
        out_args.push_back(in_args.at(i) + ".generated.cpp");
        i++;
      }
    }

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

  XLOG(DBG4) << "main...";

  //CXTPL::core::Generator template_engine;

  //XLOG(DBG4) << "CXTPL::core::Generator...";

  /*template_engine.tags().code_append_raw.open_tag = CXTPL::core::SingleTag{
    CXTPL_TAG_OPENING "sadasd", "", CXTPL_TAG_CLOSING,
  };*/

#if 0
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
#endif // 0

  srcdir_abs_path = fs::absolute(fs::current_path());
  if(srcdir_arg.is_initialized() && !srcdir_arg.value().empty()) {
    srcdir_abs_path = fs::absolute(fs::path(srcdir_arg.value()));
    if(!fs::is_directory(srcdir_abs_path)) {
      XLOG(ERR) << srcdir_arg.value() << " must be directory";
      return EXIT_FAILURE;
    }
    fs::current_path(srcdir_arg.value());
  }

  resdir_abs_path = fs::absolute(fs::current_path());
  if(resdir_arg.is_initialized() && !resdir_arg.value().empty()) {
    resdir_abs_path = fs::absolute(fs::path(resdir_arg.value()));
    if(!fs::is_directory(resdir_abs_path)) {
      XLOG(ERR) << resdir_arg.value() << " must be directory";
      return EXIT_FAILURE;
    }
    fs::current_path(resdir_arg.value());
  }

  auto chrono_then = std::chrono::steady_clock::now();

  run_generation();

  long int diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();
  long int diff_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();
  XLOG(DBG4) << "Done in : " << diff_ms << " milliseconds (" << diff_ns << " nanoseconds)";
  return EXIT_SUCCESS;
}
