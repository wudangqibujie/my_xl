//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_SOLVER_H
#define MY_XL_SOLVER_H

#include "../base/common.h"
#include "../base/thread_pool.h"
#include "../data/hyper_parameters.h"
#include "../data/data_structure.h"
#include "../data/model_parameters.h"
#include "../reader/reader.h"
#include "../reader/parser.h"
#include "../reader/file_splitor.h"
#include "../score/score_function.h"
#include "../loss/loss.h"
#include "../loss/metric.h"
#include "../solver/checker.h"
#include "../solver/trainer.h"
#include "../solver/inference.h"
#include "checker.h"
namespace xLearn {
//------------------------------------------------------------------------------
// Solver is entry class of xLearn, which can perform training
// or prediction tasks. There are three important functions in this
// class, including the Initialize(), StartWork(), and Clear() funtions.
//
// We can use Solver class like this:
//
//  xLearn::Solver solver;
//  solver.SetTrain();   // or solver.SetPredict()
//  solver.Initialize(argc, argv);
//  solver.StartWork();
//  solver.Clear();
//------------------------------------------------------------------------------
    class Solver {
    public:
        // Constructor and Destructor
        Solver()
                : score_(nullptr),
                  loss_(nullptr),
                  metric_(nullptr) { }
        ~Solver() { }

        // Ser train or predict
        void SetTrain() { hyper_param_.is_train = true; }
        void SetPredict() { hyper_param_.is_train = false; }

        // Initialize the xLearn environment, including checking
        // and parsing the commad line arguments, reading problem
        // (training data or testing data), initialize model, loss,
        // metric, and score functions, etc.
        void Initialize(int argc, char* argv[]);

        // Initialize the xLearn environment through the
        // given hyper-parameters. This function will be
        // used for python API.
        void Initialize(HyperParam& hyper_param);

        // Start a training task or start an inference task.
        void StartWork();

        // Clear the xLearn environment.
        void Clear();

    protected:
        /* Global hyper-parameters */
        xLearn::HyperParam hyper_param_;
        /* Check the user input */
        xLearn::Checker checker_;
        /* Global model parameters */
        xLearn::Model* model_;
        /* One Reader corresponds one data file */
        std::vector<xLearn::Reader*> reader_;
        /* Split file in cross-validation */
        xLearn::FileSpliter splitor_;
        /* linear, fm or ffm ? */
        xLearn::Score* score_;
        /* cross-entropy or squared ? */
        xLearn::Loss* loss_;
        /* acc, prec, recall, mae, etc */
        xLearn::Metric* metric_;
        /* ThreadPool for multi-thread training */
        ThreadPool* pool_;

        // Create object by name
        xLearn::Reader* create_reader();
        xLearn::Score* create_score();
        xLearn::Loss* create_loss();
        xLearn::Metric* create_metric();

        // xLearn command line logo
        void print_logo() const;

        // Initialize function
        void init_train();
        void init_predict();
        void init_log();
        void checker(int argc, char* argv[]);
        void checker(HyperParam& hyper_param);

        // Start function
        void start_train_work();
        void start_prediction_work();

    };

} // namespace xLearn


#endif //MY_XL_SOLVER_H
