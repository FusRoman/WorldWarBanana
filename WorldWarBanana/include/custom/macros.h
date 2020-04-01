#pragma once

#ifndef PROD
#include <iostream>
#define DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#define WARNING(msg) std::cout << "[WARNING] " << msg << std::endl;
#else
#define DEBUG(msg)
#define WARNING(msg)
#endif
