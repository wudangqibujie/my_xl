//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_CROSS_ENTROPY_LOSS_H
#define MY_XL_CROSS_ENTROPY_LOSS_H

#include "../base/common.h"
#include "../loss/loss.h"

namespace xLearn {

//------------------------------------------------------------------------------
// CrossEntropyLoss is used for classification tasks, which
// has the following form:
// loss = sum_all_example(log(1.0+exp(-y*pred)))
//------------------------------------------------------------------------------
    class CrossEntropyLoss : public Loss {
    public:
        // Constructor and Desstructor
        CrossEntropyLoss() { }
        ~CrossEntropyLoss() { }

        // Given predictions and labels, accumulate cross-entropy loss.
        void Evalute(const std::vector<real_t>& pred,
                     const std::vector<real_t>& label);

        // Given data sample and current model, calculate gradient
        // and update current model parameters.
        // This function will also accumulate the loss value.
        void CalcGrad(const DMatrix* data_matrix, Model& model);

        // Return current loss type.
        std::string loss_type() { return "log_loss"; }

    private:
        DISALLOW_COPY_AND_ASSIGN(CrossEntropyLoss);
    };

}  // namespace xLearn
#endif //MY_XL_CROSS_ENTROPY_LOSS_H
