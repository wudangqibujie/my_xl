//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_SQUARED_LOSS_H
#define MY_XL_SQUARED_LOSS_H

#include "../base/common.h"
#include "../loss/loss.h"

namespace xLearn {

//------------------------------------------------------------------------------
// SquaredLoss is used for regression tasks in machine learning, which
// has the following form:
// loss = sum_all_example( (y - pred) ^ 2 )
//------------------------------------------------------------------------------
    class SquaredLoss : public Loss {
    public:
        // Constructor and Desstructor
        SquaredLoss() { };
        ~SquaredLoss() { }

        // Given predictions and labels, accumulate loss value.
        void Evalute(const std::vector<real_t>& pred,
                     const std::vector<real_t>& label);

        // Given data sample and current model, calculate gradient
        // and update current model parameters.
        // This function will also accumulate the loss value.
        void CalcGrad(const DMatrix* data_matrix, Model& model);

        // Return current loss type
        std::string loss_type() { return "mse_loss"; }

    private:
        DISALLOW_COPY_AND_ASSIGN(SquaredLoss);
    };

} // namespace xLearn
#endif //MY_XL_SQUARED_LOSS_H
