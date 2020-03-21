#pragma once

#ifndef PROD
#include <iostream>
#define DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
#define DEBUG(msg)
#endif
