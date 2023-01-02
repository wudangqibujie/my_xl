//
// Created by jay on 2023/1/2.
//

#include "../loss/metric.h"

namespace xLearn {

    CLASS_REGISTER_IMPLEMENT_REGISTRY(xLearn_metric_registry, Metric);
    REGISTER_METRIC("acc", AccMetric);
    REGISTER_METRIC("prec", PrecMetric);
    REGISTER_METRIC("recall", RecallMetric);
    REGISTER_METRIC("f1", F1Metric);
    REGISTER_METRIC("mae", MAEMetric);
    REGISTER_METRIC("mape", MAPEMetric);
    REGISTER_METRIC("rmsd", RMSDMetric);
    REGISTER_METRIC("auc", AUCMetric);

}  // namespace xLearn
