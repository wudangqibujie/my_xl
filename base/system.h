//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_SYSTEM_H
#define MY_XL_SYSTEM_H

//#include <sys/utsname.h>
#include <unistd.h>
#include "utsname.h"

#include <string>

#include "../base/common.h"
#include "../base/stringprintf.h"

// Get host name
std::string get_host_name() {
    struct utsname buf;
    if (0 != uname(&buf)) {
        *buf.nodename = '\0';
    }
    return std::string(buf.nodename);
}

// Get user name
std::string get_user_name() {
    const char* username = getenv("USER");
    return username != NULL ? username : getenv("USERNAME");
}

// Get current system time
std::string print_current_time() {
    time_t current_time = time(NULL);
    struct tm broken_down_time;
//    CHECK(localtime_r(&current_time, &broken_down_time) == &broken_down_time);
    return StringPrintf("%04d%02d%02d-%02d%02d%02d",
                        1900 + broken_down_time.tm_year,
                        1 + broken_down_time.tm_mon,
                        broken_down_time.tm_mday,
                        broken_down_time.tm_hour,
                        broken_down_time.tm_min,
                        broken_down_time.tm_sec);
}

// The log file name = base + host_name + username +
//                     date_time + process_id
std::string get_log_file(const std::string& file_base) {
    CHECK(!file_base.empty());
    std::string filename_prefix;
    SStringPrintf(&filename_prefix,
                  "%s.%s.%s.%s.%u",
                  file_base.c_str(),
                  get_host_name().c_str(),
                  get_user_name().c_str(),
                  print_current_time().c_str(),
                  getpid());
    return filename_prefix;
}

#endif //MY_XL_SYSTEM_H
