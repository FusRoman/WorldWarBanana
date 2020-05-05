#pragma once

#include <iostream>

#ifndef PROD
#define DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#define WARNING(msg) std::cout << "[WARNING] " << msg << std::endl
#else
#define DEBUG(msg)
#define WARNING(msg)
#endif

// Le fait qu'il s'agit de plusieurs instructions pourrait poser problème si on oublie de mettre les accollades
#define ERROR(msg) {std::cerr << "[ERROR] " << msg << std::endl; exit(1);}
