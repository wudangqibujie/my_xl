#include <iostream>
#include "base/timer.h"
#include "solver/solver.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    printf("Argc: %d\n", argc);
    for (int i = 0; i < argc; ++i) {
        printf("arg %d is: %s\n", i, argv[i]);
    }

    Timer timer;
    timer.tic();

    xLearn::Solver solver;
    solver.SetTrain();
    solver.Initialize(argc, argv);
    solver.StartWork();
    solver.Clear();

    print_info(
            StringPrintf("Total time cost: %.2f (sec)",
                         timer.toc()), false);
    return 0;
}
