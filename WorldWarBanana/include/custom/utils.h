#pragma once

#include <list>
#include "Sound.h"

typedef unsigned int uint;

/**
 * @brief Retourne un entier dans [lower, upper].
 */
int randomInt(int lower, int upper);

/**
 * @brief Retourne un flottant dans [low, upper[.
 */
float randomFloat(float lower, int upper);

template<class T>
T max(T a, T b)
{
    return (a > b)? a : b;
}

template<class T>
T min(T a, T b)
{
    return (a < b)? a : b;
}

/**
 * @brief Convertit une liste en un array avec la bonne taille.
 * La destruction de l'array (avec delete[]) est laissé à la charge de l'appelant.
 * Tous les éléments sont copiés, donc le type T doit avoir un constructeur vide 
 * et un opérateur = valide.
 */
template<class T>
T* toArray(const std::list<T>& list)
{
    T* result = new T[list.size()];
    int i = 0;
    for (const T& el: list)
    {
        result[i] = el;
        ++i;
    }
    return result;
}

/**
 * @brief Renvoie true si c est un espace, une tabulation ou un retour de ligne.
 */
bool isSpace(char c);

/**
 * @brief Renvoie true si c est un chiffre.
 */
bool isNumeric(char c);

/**
 * @brief Renvoie true si c est un caractère alphabétique, majuscule ou
 * minuscule.
 */
bool isAlpha(char c);

/**
 * @brief Renvoie true si c est un caractère alphabétique minuscule.
 */
bool isLowerAlpha(char c);

/**
 * @brief Renvoie true si c est un caractère alphabétique majuscule.
 */
bool isUpperAlpha(char c);

/**
 * @brief Joue un son uniquement s'il ne vaut pas null.
 */
inline void play(Sound* s)
{
    if (s) s->play();
}