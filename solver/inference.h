//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_INFERENCE_H
#define MY_XL_INFERENCE_H

#include <string>

#include "../base/common.h"
#include "../data/data_structure.h"
#include "../data/model_parameters.h"
#include "../reader/reader.h"
#include "../loss/loss.h"

namespace xLearn {

//------------------------------------------------------------------------------
// Given a pre-trained model and test data, the predictor
// will return the prediction output
//------------------------------------------------------------------------------
    class Predictor {
    public:
        // Constructor and Desstructor
        Predictor() { }
        ~Predictor() { }

        // Invoke this function before we use this class
        void Initialize(Reader* reader,
                        Model* model,
                        Loss* loss,
                        const std::string& out,
                        bool sign = false,
                        bool sigmoid = false) {
            CHECK_NOTNULL(reader);
            CHECK_NOTNULL(model);
            CHECK_NOTNULL(loss);
            CHECK_NE(out.empty(), true);
            reader_ = reader;
            model_ = model;
            loss_ = loss;
            out_file_ = out;
            sign_ = sign;
            sigmoid_ = sigmoid;
        }

        // The core function
        void Predict();

    protected:
        Reader* reader_;
        Model* model_;
        Loss* loss_;
        std::string out_file_;
        bool sign_;
        bool sigmoid_;

        // Convert output by using the sigmoid function.
        void sigmoid(std::vector<real_t>& in,
                     std::vector<real_t>& out);

        // Convert output to 0 and 1.
        void sign(std::vector<real_t>& in,
                  std::vector<real_t>& out);

    private:
        DISALLOW_COPY_AND_ASSIGN(Predictor);
    };  // class Predictor

}  // namespace xLearn

#endif //MY_XL_INFERENCE_H
