//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_LINEAR_SCORE_H
#define MY_XL_LINEAR_SCORE_H

#include "../base/common.h"
#include "../data/model_parameters.h"
#include "../score/score_function.h"

namespace xLearn {

//------------------------------------------------------------------------------
// LinearScore is used to implemente generalized linear
// models (GLMs), where the socre function is y = wTx.
//------------------------------------------------------------------------------
    class LinearScore : public Score {
    public:
        // Constructor and Desstructor
        LinearScore() { }
        ~LinearScore() { }

        // Given one exmaple and current model, this method
        // returns the linear score wTx.
        real_t CalcScore(const SparseRow* row,
                         Model& model,
                         real_t norm = 1.0);

        // Calculate gradient and update current
        // model parameters.
        void CalcGrad(const SparseRow* row,
                      Model& model,
                      real_t pg,
                      real_t norm = 1.0);

    protected:
        // Calculate gradient and update model using sgd
        void calc_grad_sgd(const SparseRow* row,
                           Model& model,
                           real_t pg,
                           real_t norm = 1.0);

        // Calculate gradient and update model using adagrad
        void calc_grad_adagrad(const SparseRow* row,
                               Model& model,
                               real_t pg,
                               real_t norm = 1.0);

        // Calculate gradient and update model using ftrl
        void calc_grad_ftrl(const SparseRow* row,
                            Model& model,
                            real_t pg,
                            real_t norm = 1.0);

    private:
        DISALLOW_COPY_AND_ASSIGN(LinearScore);
    };

}  // namespace xLearn

#endif //MY_XL_LINEAR_SCORE_H
