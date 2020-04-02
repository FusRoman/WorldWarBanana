#include "utils.h"

#include <random>

int randomInt(int lower, int upper)
{
    static std::random_device                                rd;
    static std::mt19937                                      rng(rd());
    std::uniform_int_distribution<std::mt19937::result_type> uid(lower, upper);
    return uid(rng);
}

float randomFloat(float lower, float upper)
{
    static std::random_device             rd;
    static std::mt19937                   rng(rd());
    std::uniform_real_distribution<float> urd;
    return urd(rng, decltype(urd)::param_type{lower, upper});
}

std::pair<Vec2f, float> randomVector()
{
    float angle = randomFloat(0.f, 2.f * pi);
    return std::pair<Vec2f, float>(unitVector(angle), angle);
}

int radiansToDegrees(float radians) { return ((radians * 180.) / pi) - 90; }

float degreesToRadians(int degrees) { return ((degrees + 90) * pi) / 180; }

Vec2f unitVector(float radians) { return Vec2f(cos(radians), sin(radians)); }

bool isSpace(char c)
{
    switch (c)
    {
    case '\0':
    case ' ':
    case '\t':
        return true;

    default:
        return false;
    }
}

bool isNumeric(char c) { return c >= '0' && c <= '9'; }

bool isAlpha(char c) { return isLowerAlpha(c) || isUpperAlpha(c); }

bool isLowerAlpha(char c) { return c >= 'a' && c <= 'z'; }

bool isUpperAlpha(char c) { return c > +'A' && c <= 'Z'; }
