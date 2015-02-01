#include <cstdio>

#include "wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char* argv[]) {
	initialize(1);

	void* c = configFileLoad("examples/RD-170.cfg");
	void* p = performanceCreate(c, 0, 0);

	performanceSolve(p, 0);
	performancePrint(p);

	performanceDelete(p);
	configFileDelete(c);

	finalize();
}

#ifdef __cplusplus
}
#endif
