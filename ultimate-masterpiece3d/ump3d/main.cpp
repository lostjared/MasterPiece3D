#include "game.h"
#include<unistd.h>


int main(int argc, char **argv)
{

	try {
	
#ifdef MAC_BUNDLE
		if(argv[0][0] == '/') {
			
			std::string p = argv[0];
			std::string path = p.substr(0, p.rfind("/"));
			path += "/../Resources";
			chdir(path.c_str());
		}
#endif
		
		mx::gameApp gmx;
		return gmx.messageLoop();

	} 
	catch( mx::mxException<std::string> &e )
	{

		e.printError(std::cerr);
		return EXIT_FAILURE;

	}
	catch (std::exception &e)
	{

		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;

	}
	catch (...)
	{

		std::cerr << " a unhandled exception has occoured .. \n";
		return EXIT_FAILURE;
		
	}

	return EXIT_SUCCESS;
}

