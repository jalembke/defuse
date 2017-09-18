#include "hybrid_debug.h"
#include "sptr.h"

#include <map>

std::map<int, sptr<int> > mymap;

void do_insert(int key, sptr<int>& pointer)
{
	mymap.insert(std::make_pair(key, pointer));
}

int main(int argc, char* argv[])
{
	DEBUG_PRINT("HERE 1");
	sptr<int> myint(new int);
	DEBUG_PRINT("HERE 2");
	sptr<int> myint2 = myint;
	DEBUG_PRINT("HERE 3");
	*myint = 1;
	DEBUG_PRINT("HERE 4");

	sptr<int> myint3;
	DEBUG_PRINT(myint3.use_count());

	do_insert(0, myint);
	DEBUG_PRINT(myint.use_count());

	do_insert(1, myint2);
	DEBUG_PRINT(myint.use_count());

	do_insert(2, myint);
	DEBUG_PRINT(myint.use_count());

	return 0;
}
