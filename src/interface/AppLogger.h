#ifndef INTERFACE_APPLOGGER_H
#define INTERFACE_APPLOGGER_H


#include <spdlog/spdlog.h>
#include <iostream>
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

//#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/async.h"


class AppLogger : public spdlog::logger {
public:
    AppLogger(const std::string& name, const std::string& log_file="/tmp/rfid_app.log", size_t max_size = 1048576, size_t max_files = 3)
            : spdlog::logger(name)
            {

        // Create a rotating file sink
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file, max_size, max_files);
        // Create a console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        // Add sinks
        sinks().push_back(file_sink);
        sinks().push_back(console_sink);

        set_level(spdlog::level::info);
        flush_on(spdlog::level::info);

//        set_pattern("[%Y-%m-%d %H:%M:%S.%e] [t_id %t p_id %P]  [%^%l%$] %v");

        auto combined_logger = std::make_shared<spdlog::logger>("multi", std::begin(sinks()), std::end(sinks()));
//        spdlog::set_default_logger(combined_logger);
//        set_pattern("[%n][%Y-%m-%d] [%^%l%$] %v");

        spdlog::register_logger(combined_logger);

    }

    void set_app_log_env( std::string& environment){
        // Determine the log level based on the environment
        spdlog::level::level_enum logLevel;
        if (environment == "DEBUG") {
            logLevel = spdlog::level::debug;
            set_pattern("[%^%l%$] [%n] [t_id %t p_id %P] %v");

        } else {
            logLevel = spdlog::level::info;
            set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
        }

        // Initialize the logger with the determined log level
        this->set_level(logLevel);
    }

    // Static method to get the logger instance
    static std::shared_ptr<AppLogger> getLogger(std::string environment) {
        static std::shared_ptr<AppLogger> logger = std::make_shared<AppLogger>
                ("app_logger", "/tmp/rfid_app.log");
        logger->set_app_log_env((std::string&)environment);
        return logger;
    }

protected:

};



#endif //INTERFACE_APPLOGGER_H
