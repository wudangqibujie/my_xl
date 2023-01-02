//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_FILE_SPLITOR_H
#define MY_XL_FILE_SPLITOR_H

#include <string>

#include "../base/common.h"

namespace xLearn {

//------------------------------------------------------------------------------
// Split file using mmap() on Unix-like systems.
//------------------------------------------------------------------------------
    class FileSpliter {
    public:
        FileSpliter() {  }
        ~FileSpliter() {  }

        void split(const std::string& filename, int num_blocks);

    private:
        DISALLOW_COPY_AND_ASSIGN(FileSpliter);
    };

}  // namespace xLearn

#endif //MY_XL_FILE_SPLITOR_H
