//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_CHECKER_H
#define MY_XL_CHECKER_H

#include <vector>
#include <string>

#include "../base/common.h"
#include "../data/hyper_parameters.h"


namespace xLearn {

    typedef std::vector<std::string> StringList;

//------------------------------------------------------------------------------
// Checker is used to check and parse command line arguments for xLearn.
//------------------------------------------------------------------------------
    class Checker {
    public:
        // Constructor and Destructor
        Checker() { }
        ~Checker() { }

        // Initialize Checker
        void Initialize(bool is_train, int argc, char* argv[]);

        // Check and parse arguments
        bool check_cmd(HyperParam& hyper_param);

        // Check hyper-param. Used by c_api
        bool check_param(HyperParam& hyper_param);

    protected:
        /* Store all the possible options */
        StringList menu_;
        /* User input command line */
        StringList args_;
        /* train or predict */
        bool is_train_;

        // Print the help menu
        std::string option_help() const;

        // Check options for training and prediction
        bool check_train_options(HyperParam& hyper_param);
        bool check_train_param(HyperParam& hyper_param);
        bool check_prediction_options(HyperParam& hyper_param);
        bool check_prediction_param(HyperParam& hyper_param);
        void check_conflict_train(HyperParam& hyper_param);
        void check_conflict_predict(HyperParam& hyper_param);

    private:
        DISALLOW_COPY_AND_ASSIGN(Checker);
    };

} // namespace xLearn


#endif //MY_XL_CHECKER_H
