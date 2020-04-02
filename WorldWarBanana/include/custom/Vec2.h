#pragma once

/**
 * @brief Une classe pour les vecteurs 2D de tout type.
 */
template <class T> class Vec2
{
public:
    T x;
    T y;

    Vec2(): x(0), y(0) {}

    Vec2(T x, T y): x(x), y(y) {}

    Vec2(const Vec2<T>& v): x(v.x), y(v.y) {}

    /**
     * @brief Affectation.
     */
    const Vec2<T>& operator=(const Vec2<T>& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    /**
     * @brief Addition in-place.
     */
    const Vec2<T>& operator+=(const Vec2<T>& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    /**
     * @brief Soustraction in-place.
     */
    const Vec2<T>& operator-=(const Vec2<T>& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    /**
     * @brief Multiplication in-place.
     */
    const Vec2<T>& operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    /**
     * @brief Division in-place.
     */
    const Vec2<T>& operator/=(T scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    /**
     * @brief Egalité entre deux vecteurs.
     */
    Vec2<T> operator==(const Vec2<T>& v) const { return x == v.x && y == v.y; }

    /**
     * @brief Inégalité entre deux vecteurs.
     */
    Vec2<T> operator!=(const Vec2<T>& v) const { return !(*this == v); }

    /**
     * @brief Opposé du vecteur.
     */
    Vec2<T> operator-() const { return Vec2<T>(-x, -y); }

    /**
     * @brief Addition entre deux vecteurs.
     */
    Vec2<T> operator+(const Vec2<T>& v) const { return Vec2<T>(x + v.x, y + v.y); }

    /**
     * @brief Soustraction entre deux vecteurs.
     */
    Vec2<T> operator-(const Vec2<T>& v) const { return Vec2<T>(x - v.x, y - v.y); }

    /**
     * @brief Multiplication avec un scalaire.
     */
    friend Vec2<T> operator*(const Vec2<T>& v, T scalar)
    {
        return Vec2<T>(v.x * scalar, v.y * scalar);
    }

    /**
     * @brief Overload de operator *.
     */
    friend Vec2<T> operator*(T scalar, const Vec2<T>& v) { return v * scalar; }

    /**
     * @brief Division par un scalaire.
     */
    friend Vec2<T> operator/(const Vec2<T>& v, T scalar)
    {
        return Vec2<T>(v.x / scalar, v.y / scalar);
    }

    /**
     * @brief Affiche le vecteur sur os.
     */
    friend std::ostream& operator<<(std::ostream& os, const Vec2<T>& v)
    {
        return os << "(" << v.x << ", " << v.y << ")";
    }

    /**
     * @brief Renvoie la norme du vecteur.
     */
    float norm() const { return sqrt(x * x + y * y); }

    /**
     * @brief Produit scalaire.
     */
    T dot(const Vec2<T>& v) const { return x * v.x + y * v.y; }

    /**
     * @brief Déterminant.
     */
    T det(const Vec2<T>& v) const { return x * v.y - y * v.x; }

    /**
     * @brief Retourne un vecteur unitaire de même direction.
     * Cette fonction ne fonctionne correctement que pour les types flottants.
     */
    Vec2<T> normalize() const { return *this / norm(); }

    /**
     * @brief Renvoie l'angle entre ce vecteur et (0, 1).
     */
    float angle() const { return atan2(y, x); }

    /**
     * @brief Renvoie l'angle en radians entre deux vecteurs.
     */
    float angle(const Vec2<T>& v) const { return atan2(dot(v), det(v)); }
};

typedef Vec2<int>   Vec2i;
typedef Vec2<float> Vec2f;
