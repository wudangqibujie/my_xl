//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_PARSER_H
#define MY_XL_PARSER_H

#include <vector>
#include <string>

#include "../base/common.h"
#include "../base/class_register.h"
#include "../data/data_structure.h"

namespace xLearn {

//------------------------------------------------------------------------------
// Given a memory buffer, parse it to the DMatrix format.
// Parser is an abstract class, which can be implemented by real
// Parser such as the LibsvmParser, FFMParser, and CSVParser.
// We can use the Parser class like this:
//
//   std::string filename = "/tmp/train.txt";
//   std::string format = CheckFileFormat(filename);
//   Parser* parser = nullptr;
//   if (format == "libsvm") {
//     parser = new LibsvmParser();
//   } else if (format == "ffm") {
//     parser = new FFMParser();
//   } else {
//     parser = new CSVParser();
//   }
//   parser->setLabel(true);  // this dataset contains label y
//   char* buffer = nullptr;
//   uint64 size = ReadFileToMemory(filename, buffer);
//   DMatrix matrix;
//   parser->Parse(buffer, size, matrix);
//------------------------------------------------------------------------------
    class Parser {
    public:
        Parser() { }
        virtual ~Parser() {  }

        // Wether this dataset contains label y ?
        inline void setLabel(bool label) {
            has_label_ = label;
        }

        // The real parse function invoked by users.
        virtual void Parse(char* buf, uint64 size, DMatrix& matrix) = 0;

    protected:
        // Get how many lines in current memory buffer.
        index_t get_line_number(char* buf, uint64 size);

        // Get one line from memory buffer.
        uint64 get_line_from_buffer(char* line,
                                    char* buf,
                                    uint64 pos,
                                    uint64 size);

        /* True for training task and
        False for prediction task */
        bool has_label_;

    private:
        DISALLOW_COPY_AND_ASSIGN(Parser);
    };

//------------------------------------------------------------------------------
// LibsvmParser parses the following data format:
// [y1 idx:value idx:value ...]
// [y2 idx:value idx:value ...]
//------------------------------------------------------------------------------
    class LibsvmParser : public Parser {
    public:
        LibsvmParser() { }
        ~LibsvmParser() {  }

        // Parse the libsvm file
        void Parse(char* buf, uint64 size, DMatrix& matrix);

    private:
        DISALLOW_COPY_AND_ASSIGN(LibsvmParser);
    };

//------------------------------------------------------------------------------
// FFMParser parses the following data format:
// [y1 field:idx:value field:idx:value ...]
// [y2 field:idx:value field:idx:value ...]
//------------------------------------------------------------------------------
    class FFMParser : public Parser {
    public:
        FFMParser() { }
        ~FFMParser() {  }

        // Parse the libffm file
        void Parse(char* buf, uint64 size, DMatrix& matrix);

    private:
        DISALLOW_COPY_AND_ASSIGN(FFMParser);
    };

//------------------------------------------------------------------------------
// CSVParser parses the following data format:
// [feat_1 feat_2 feat_3 ... feat_n y1]
// [feat_1 feat_2 feat_3 ... feat_n y2]
// Note that, if the csv data doesn't contain the
// label y, the user should add a placeholder to the dataset
// by themselves. Otherwise, the parser will treat the last
// element as the label y.
//------------------------------------------------------------------------------
    class CSVParser : public Parser {
    public:
        CSVParser() { }
        ~CSVParser() { }

        // Parse the csv file
        void Parse(char* buf, uint64 size, DMatrix& matrix);

    private:
        DISALLOW_COPY_AND_ASSIGN(CSVParser);
    };

//------------------------------------------------------------------------------
// Class register
//------------------------------------------------------------------------------
    CLASS_REGISTER_DEFINE_REGISTRY(xLearn_parser_registry, Parser);

#define REGISTER_PARSER(format_name, parser_name)          \
  CLASS_REGISTER_OBJECT_CREATOR(                           \
      xLearn_parser_registry,                              \
      Parser,                                              \
      format_name,                                         \
      parser_name)

#define CREATE_PARSER(format_name)                         \
  CLASS_REGISTER_CREATE_OBJECT(                            \
      xLearn_parser_registry,                              \
      format_name)

}  // namespace xLearn

#endif //MY_XL_PARSER_H
