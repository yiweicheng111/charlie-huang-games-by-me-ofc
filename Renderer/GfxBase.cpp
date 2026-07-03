#include "GfxBase.h"
#include <iostream>

std::string Cle::Gfx::readFile(std::string path)
{
	std::ifstream file(path, std::ios::in);
	std::stringstream s;
	s << file.rdbuf();
	return s.str();
}