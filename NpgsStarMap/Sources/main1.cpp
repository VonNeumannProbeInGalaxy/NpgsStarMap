
#include "StarMap.h"

int main(int argc, char* args[]) {
	try {
		StarMap star_map(1000, 600);
		star_map.menu();

	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}