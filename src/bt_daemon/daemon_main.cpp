#include "bt_daemon/daemon.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Daemon {

int bt_daemon_main(int argc, char *argv[])
{
    //Entry point for separate executable?
    return 0;
}


int btd_daemon()
{
    pid_t pid;
    pid = fork();


    if (pid == -1) {
        spdlog::error("Failed to fork() [daemon]");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        spdlog::info("Daemon launched successfully");
        exit(EXIT_SUCCESS);
    }

    // in child proc proper
    try {
        auto logger = spdlog::basic_logger_mt("btd_daemon", "daemon.log");
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex& ex) {
        creat("daemon_log_failed", 0666);
    }

    spdlog::info("Daemon started");

    exit(EXIT_SUCCESS);
}

} //end namespace Daemon
