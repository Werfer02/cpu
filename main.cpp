#include "app.hpp"

int main(int argc, char** argv){
	App a;
	a.configure(argc, argv);
	a.run();
}
