//
// Created by jay on 2023/1/2.
//

#include "../solver/inference.h"
#include "../base/timer.h"
#include "../base/format_print.h"

#include <vector>
#include <sstream>

namespace xLearn {

// Given a pre-trained model and test data, the predictor
// will return the prediction output
    void Predictor::Predict() {
        std::ofstream o_file(out_file_);
        static std::vector<real_t> out;
        DMatrix* matrix = nullptr;
        reader_->Reset();
        loss_->Reset();
        for (;;) {
            index_t tmp = reader_->Samples(matrix);
            if (tmp == 0) { break; }
            if (tmp != out.size()) { out.resize(tmp); }
            loss_->Predict(matrix, *model_, out);
            if (reader_->has_label()) {
                loss_->Evalute(out, matrix->Y);
            }
            if (sigmoid_) {
                this->sigmoid(out, out);
            } else if (sign_) {
                this->sign(out, out);
            }
            for (index_t i = 0; i < out.size(); ++i) {
                o_file << out[i] << "\n";
            }
        }
        if (reader_->has_label()) {
            print_info(
                    StringPrintf("The test loss is: %.6f",
                                 loss_->GetLoss())
            );
        }
    }

// Convert output by using the sigmoid function.
    void Predictor::sigmoid(std::vector<real_t>& in,
                            std::vector<real_t>& out) {
        CHECK_EQ(in.size(), out.size());
        for (size_t i = 0; i < in.size(); ++i) {
            out[i] = 1.0 / (1.0 + exp(-in[i]));
        }
    }

// Convert output to 0 and 1.
    void Predictor::sign(std::vector<real_t>& in,
                         std::vector<real_t>& out) {
        CHECK_EQ(in.size(), out.size());
        for (size_t i = 0; i < in.size(); ++i) {
            out[i] = in[i] > 0 ? 1 : 0;
        }
    }

}  // namespace xLearn
