//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_FM_SCORE_H
#define MY_XL_FM_SCORE_H

#include "../base/common.h"
#include "../data/model_parameters.h"
#include "../score/score_function.h"

namespace xLearn {

//------------------------------------------------------------------------------
// FMScore is used to implemente factorization machines, in which
// the socre function is y = sum( (V_i*V_j)(x_i * x_j) )
// Here we leave out the linear term and bias term.
//------------------------------------------------------------------------------
    class FMScore : public Score {
    public:
        // Constructor and Desstructor
        FMScore() { }
        ~FMScore() { }

        // Given one exmaple and current model, this method
        // returns the fm score.
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
        real_t* comp_res = nullptr;
        real_t* comp_z_lt_zero = nullptr;
        real_t* comp_z_gt_zero = nullptr;

    private:
        DISALLOW_COPY_AND_ASSIGN(FMScore);
    };

} // namespace xLearn

#endif //MY_XL_FM_SCORE_H
