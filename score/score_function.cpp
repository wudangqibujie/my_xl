//
// Created by jay on 2023/1/2.
//

#include "../score/score_function.h"
#include "../score/linear_score.h"
#include "../score/fm_score.h"
#include "../score/ffm_score.h"

namespace xLearn {

//------------------------------------------------------------------------------
// Class register
//------------------------------------------------------------------------------
    CLASS_REGISTER_IMPLEMENT_REGISTRY(xLearn_score_registry, Score);
    REGISTER_SCORE("linear", LinearScore);
    REGISTER_SCORE("fm", FMScore);
    REGISTER_SCORE("ffm", FFMScore);

}  // namespace xLearn