//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_FFM_SCORE_H
#define MY_XL_FFM_SCORE_H

#include "../base/common.h"
#include "../score/score_function.h"

namespace xLearn {

//------------------------------------------------------------------------------
// FFMScore is used to implemente field-aware factorization machines,
// in which the socre function is:
//   y = sum( (V_i_fj*V_j_fi)(x_i * x_j) )
// Here leave out the bias and linear term.
//------------------------------------------------------------------------------
    class FFMScore : public Score {
    public:
        // Constructor and Desstructor
        FFMScore() { }
        ~FFMScore() { }

        // Given one exmaple and current model, this method
        // returns the ffm score.
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
        real_t* comp_res1 = nullptr;
        real_t* comp_res2 = nullptr;
        real_t* comp_z_lt_zero = nullptr;
        real_t* comp_z_gt_zero = nullptr;

    private:
        DISALLOW_COPY_AND_ASSIGN(FFMScore);
    };

}  // namespace xLearn
#endif //MY_XL_FFM_SCORE_H
