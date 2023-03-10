//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_FILE_UTIL_H
#define MY_XL_FILE_UTIL_H

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include "common.h"
#include <algorithm>
#include <vector>
#include "scoped_ptr.h"
#include "stringprintf.h"
const size_t KB = 1024.0;
const size_t MB = 1024.0 * 1024.0;
const size_t GB = 1024.0 * 1024.0 * 1024.0;

/* 500 KB for one line of txt data */
static const uint32 kMaxLineSize = 500 * 1024;  // 500 KB
/* Chunk size for hash block */
static const uint32 kChunkSize = 1000 * 1024; // 1000 KB

// Check whether the file exists.
inline bool FileExist(const char* filename) {
    if (access(filename, F_OK) != -1) {
        return true;
    }
    LOG(WARNING) << "File: " << filename << "doesn't exists.";
    return false;
}

// Open file using fopen() and return the file pointer.
// mode : "w" for write and "r" for read
inline FILE* OpenFileOrDie(const char* filename, const char* mode) {
    FILE* input_stream = fopen(filename, mode);
    if (input_stream == nullptr) {
        LOG(FATAL) << "Cannot open file: " << filename
                   << " with mode: " << mode;
    }
    return input_stream;
}

// Close file using fclose().
inline void Close(FILE *file) {
    if (fclose(file) == -1) {
        LOG(FATAL) << "Error invoke fclose().";
    }
}

// Return the size (byte) of a target file.
inline uint64 GetFileSize(FILE* file) {
    if (fseek(file, 0L, SEEK_END) != 0) {
        LOG(FATAL) << "Error: invoke fseek().";
    }
    uint64 total_size = ftell(file);
    if (total_size == -1) {
        LOG(FATAL) << "Error: invoke ftell().";
    }
    // Return to the head of file
    rewind(file);
    return total_size;
}

// Get one line of data from the file.
inline void GetLine(FILE* file, std::string& str_line) {
    CHECK_NOTNULL(file);
    //static char* line = new char[kMaxLineSize];
    scoped_array<char> line(new char[kMaxLineSize]);
    CHECK_NOTNULL(fgets(line.get(), kMaxLineSize, file));
    int read_len = strlen(line.get());
    if (line[read_len - 1] != '\n') {
        LOG(FATAL) << "Encountered a too-long line.    Please check the data.";
    } else {
        line[read_len - 1] = '\0';
        // Handle the format in DOS and windows
        if (read_len > 1 && line[read_len - 2] == '\r') {
            line[read_len - 2] = '\0';
        }
    }
    str_line.assign(line.get());
}

// Write data from a buffer to disk file.
// Return the size (byte) of data we write.
inline size_t WriteDataToDisk(FILE* file, const char* buf, size_t len) {
    CHECK_NOTNULL(file);
    CHECK_NOTNULL(buf);
    CHECK_GE(len, 0);
    size_t write_len = fwrite(buf, 1, len, file);
    if (write_len != len) {
        LOG(FATAL) << "Error: invoke fwrite().";
    }
    return write_len;
}

// Read data from disk file to a buffer.
// Return the data size (byte) we have read.
// If we reach the end of the file, return 0.
inline size_t ReadDataFromDisk(FILE* file, char* buf, size_t len) {
    CHECK_NOTNULL(file);
    CHECK_NOTNULL(buf);
    CHECK_GE(len, 0);
    /* Reach the end of the file */
    if (feof(file)) {
        return 0;
    }
    size_t ret = fread(buf, 1, len, file);
    if (ret > len) {
        LOG(FATAL) << "Error: invoke fread().";
    }
    return ret;
}

// Delete target file from disk.
inline void RemoveFile(const char* filename) {
    CHECK_NOTNULL(filename);
    if (remove(filename) == -1) {
        LOG(FATAL) << "Error: invoke remove().";
    }
}

// Print file size.
inline std::string PrintSize(uint64 file_size) {
    std::string res;
    if (file_size > GB) {
        SStringPrintf(&res, "%.2f GB",
                      (double) file_size / GB);
    } else if (file_size > MB) {
        SStringPrintf(&res, "%.2f MB",
                      (double) file_size / MB);
    } else {
        SStringPrintf(&res, "%.2f KB",
                      (double) file_size / KB);
    }
    return res;
}

//------------------------------------------------------------------------------
// Serialize or Deserialize for std::vector and std::string
//------------------------------------------------------------------------------

// Write a std::vector to disk file.
template <typename T>
void WriteVectorToFile(FILE* file_ptr, const std::vector<T>& vec) {
    CHECK_NOTNULL(file_ptr);
    // We do not allow to Serialize an empty vector
    CHECK(!vec.empty());
    size_t len = vec.size();
    WriteDataToDisk(file_ptr,
                    reinterpret_cast<char*>(&len),
                    sizeof(len));
    WriteDataToDisk(file_ptr,
                    (char*)(vec.data()),
                    sizeof(T)*len);
}

// Read a std::vector from disk file.
template <typename T>
void ReadVectorFromFile(FILE* file_ptr, std::vector<T>& vec) {
    CHECK_NOTNULL(file_ptr);
    // First, read the size of vector
    size_t len = 0;
    ReadDataFromDisk(file_ptr,
                     reinterpret_cast<char*>(&len),
                     sizeof(len));
    CHECK_GT(len, 0);
    std::vector<T>().swap(vec);
    vec.resize(len);
    ReadDataFromDisk(file_ptr,
                     reinterpret_cast<char*>(vec.data()),
                     sizeof(T)*len);
}

// Write a std::string to disk file.
inline void WriteStringToFile(FILE* file_ptr, const std::string& str) {
    CHECK_NOTNULL(file_ptr);
    // We do not allow Serialize an empty string
    CHECK(!str.empty());
    size_t len = str.size();
    WriteDataToDisk(file_ptr,
                    reinterpret_cast<char*>(&len),
                    sizeof(len));
    WriteDataToDisk(file_ptr,
                    const_cast<char*>(str.data()),
                    len);
}

// Read a std::string from disk file.
inline void ReadStringFromFile(FILE* file_ptr, std::string& str) {
    CHECK_NOTNULL(file_ptr);
    // First, read the size of vector
    size_t len = 0;
    ReadDataFromDisk(file_ptr,
                     reinterpret_cast<char*>(&len),
                     sizeof(len));
    CHECK_GT(len, 0);
    std::string().swap(str);
    str.resize(len);
    ReadDataFromDisk(file_ptr,
                     const_cast<char*>(str.data()),
                     len);
}

//------------------------------------------------------------------------------
// Some tool functions used by Reader
//------------------------------------------------------------------------------

// Calculate the hash value of current txt file.
// If one_block == true, we just read a small chunk of data.
// If one_block == false, we read all the data from the file.
inline uint64_t HashFile(const std::string& filename, bool one_block=false) {
    std::ifstream f(filename, std::ios::ate | std::ios::binary);
    if(f.bad()) { return 0; }

    long end = (long) f.tellg();
    f.seekg(0, std::ios::beg);
    CHECK_EQ(static_cast<int>(f.tellg()), 0);

    uint64_t magic = 90359;
    for(long pos = 0; pos < end; ) {
        long next_pos = std::min((long)pos + (long)kChunkSize, (long)end);
        long size = next_pos - pos;
        std::vector<char> buffer(kChunkSize);
        f.read(buffer.data(), size);

        int i = 0;
        while(i < size - 8) {
            uint64_t x = *reinterpret_cast<uint64_t*>(buffer.data() + i);
            magic = ( (magic + x) * (magic + x + 1) >> 1) + x;
            i += 8;
        }
        for(; i < size; i++) {
            char x = buffer[i];
            magic = ( (magic + x) * (magic + x + 1) >> 1) + x;
        }

        pos = next_pos;
        if(one_block) { break; }
    }

    return magic;
}

// Read the whole file to a memory buffer and
// Return size (byte) of current file.
inline uint64 ReadFileToMemory(const std::string& filename, char **buf) {
    CHECK(!filename.empty());
    FILE* file = OpenFileOrDie(filename.c_str(), "r");
    uint64 len = GetFileSize(file);
    try {
        *buf = new char[len];
    } catch (std::bad_alloc&) {
        LOG(FATAL) << "Cannot allocate enough memory for Reader.";
    }
    uint64 read_size = fread(*buf, 1, len, file);
    CHECK_EQ(read_size, len);
    Close(file);
    return len;
}

#endif //MY_XL_FILE_UTIL_H
