#include <cstdio>
#include "growingnet.h"
#include "network.h"

UL& init()
{
	static UL res = network_new(1);
	return res;
}
