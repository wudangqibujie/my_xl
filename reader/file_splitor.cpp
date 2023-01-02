//
// Created by jay on 2023/1/2.
//

//#include <sys/mman.h>
#include "mman.h"
#include <unistd.h>
#include <vector>
#include <_mingw.h>
#include "../base/common.h"
#include "../base/file_util.h"
#include "../base/stringprintf.h"
#include "../reader/file_splitor.h"

namespace xLearn {

//------------------------------------------------------------------------------
// Split file using mmap() on Unix-like systems.
//------------------------------------------------------------------------------
    void FileSpliter::split(const std::string& filename, int num_blocks) {
        CHECK_NE(filename.empty(), true);
        CHECK_GE(num_blocks, 2); // At least we need two blocks for CV.
        // Input
        FILE* file_ptr_read = OpenFileOrDie(filename.c_str(), "r");
        int file_desc_read = fileno(file_ptr_read);
        uint64 file_size = GetFileSize(file_ptr_read);
        uint64 average_block_size = file_size / num_blocks;
        uint64 next_block_size = average_block_size +
                                 file_size - (average_block_size * num_blocks);
        uint64 offset = 0;
        // Output
        FILE** file_ptr_write = new FILE* [num_blocks];
        int* file_desc_write = new int [num_blocks];

        for (int i = 0; i < num_blocks; ++i) {
            std::string name = StringPrintf("%s_%d", filename.c_str(), i);
            file_ptr_write[i] = OpenFileOrDie(name.c_str(), "w+");
            file_desc_write[i] = fileno(file_ptr_write[i]);
            int ret = ftruncate(file_desc_write[i],
                                next_block_size + kMaxLineSize);
            CHECK_EQ(ret, 0);
        }
        char* map_ptr_read = (char*)mmap(NULL,
                                         file_size,
                                         PROT_READ,
                                         MAP_PRIVATE,
                                         file_desc_read,
                                         0);
        CHECK_NE(map_ptr_read, MAP_FAILED);

        for (int i = 0; i < num_blocks; ++i) {
            char* map_ptr_write = (char *)mmap(NULL,
                                               next_block_size,
                                               PROT_WRITE,
                                               MAP_SHARED,
                                               file_desc_write[i],
                                               0);
            CHECK_NE(map_ptr_write, MAP_FAILED);
            uint64 real_file_size = next_block_size;
            memcpy(map_ptr_write, map_ptr_read+offset, real_file_size);
            while (map_ptr_write[real_file_size-1] != '\n') {
                real_file_size--;
            }
            munmap(map_ptr_write, next_block_size);
            int ret = ftruncate(file_desc_write[i], real_file_size);
            CHECK_EQ(ret, 0);
            next_block_size =
                    average_block_size + (next_block_size - real_file_size);
            offset += real_file_size;
        }
        munmap(map_ptr_read, file_size);
    }

} // namespace xLearn