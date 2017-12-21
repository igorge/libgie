//================================================================================================================================================
// FILE: logger_g3log.cpp
// (c) GIE 2017-12-21  17:25
//
//================================================================================================================================================
#include "gie/log/log.hpp"
//================================================================================================================================================
//#include "logger_g3log.hpp"
//================================================================================================================================================
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <g3log/logmessage.hpp>
//================================================================================================================================================

namespace {
    struct simple_sync_stderr_sink {

        void receive_log_message(g3::LogMessageMover &&message) {
            std::ostringstream ostr;

            ostr << message.get().timestamp("%Y/%m/%d %H:%M:%S %f8 ")
                 << std::hex << message.get()._call_thread_id
                 << ' ' << message.get().level()
                 << " [" << message.get()._file << ':' << std::dec << message.get()._line << "]["
                 << message.get().function() << "] "
                 << GIE_LOG_OPEN_TAG << message.get()._message << GIE_LOG_CLOSE_TAG
                 << '\n';

            auto const &str = ostr.str();
            std::fwrite(str.data(), 1, str.size(), stderr);
            std::fflush(stderr);
        }
    };
}

namespace gie { namespace logger {

        std::shared_ptr<void> init_logging_to_stderr(){
            auto worker = std::shared_ptr<g3::LogWorker>(g3::LogWorker::createLogWorker().release());
            worker->addSink(std::make_unique<simple_sync_stderr_sink>(), &simple_sync_stderr_sink::receive_log_message);

            g3::initializeLogging(worker.get());

            return worker;
        }

} }
//================================================================================================================================================
