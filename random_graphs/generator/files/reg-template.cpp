#include "pmodeladm.h"
#include "register.h"

{%for i in range(1,n+1)%}
#include "node{{i}}.h"{%endfor%}


void register_atomics_on(ParallelModelAdmin &admin)
{
	{%for i in range(1,n+1)%}
	admin.registerAtomic(NewAtomicFunction<Node{{i}}>(), "Node{{i}}");{%endfor%}
}
