#include "pmodeladm.h"
#include "register.h"

#include "DTMC.h"


void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction<DTMC>(), ATOMIC_MODEL_NAME);
}
