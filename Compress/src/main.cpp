#include "compress.h"

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		PNGCompress obj(argv[1]);
		obj.initialise();
		obj.compress();

	}

	

	return 0;
}