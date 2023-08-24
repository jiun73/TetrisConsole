#pragma once
#include <algorithm>
#include <iostream>
#include <tuple> 
#include <vector> 

template<class T>
class Vector2D
{
public:
	T x, y;

	Vector2D() : x(T(0)), y(T(0)) {}
	Vector2D(T p) : x(p), y(p) {}
	Vector2D(T vx, T vy) : x(vx), y(vy) {}
	Vector2D(T norm, double angle, bool polar) { setPolar(norm, angle); }
	Vector2D(const Vector2D& v) : x(v.x), y(v.y) {}

	Vector2D& operator=(const Vector2D<T>& v) { x = const_cast<T&>(v.x); y = const_cast<T&>(v.y); return *this; }
	Vector2D& operator=(const T& v) { x = v; y = v; return *this; }

	Vector2D& operator+=(const Vector2D<T>& v) { x += v.x; y += v.y; return *this; }
	Vector2D& operator-=(const Vector2D<T>& v) { x -= v.x; y -= v.y; return *this; }
	Vector2D& operator*=(const Vector2D<T>& v) { x *= v.x; y *= v.y; return *this; }
	Vector2D& operator/=(const Vector2D<T>& v) { x /= v.x; y /= v.y; return *this; }

	Vector2D& operator+=(const T& v) { x += v; y += v; return *this; }
	Vector2D& operator-=(const T& v) { x -= v; y -= v; return *this; }
	Vector2D& operator*=(const T& v) { x *= v; y *= v; return *this; }
	Vector2D& operator/=(const T& v) { x /= v; y /= v; return *this; }

	friend bool operator==(const Vector2D& v1, const Vector2D& v2) { return (v1.x == v2.x && v1.y == v2.y); }
	friend bool operator!=(const Vector2D& v1, const Vector2D& v2) { return !(v1 == v2); }

	template<class T> operator Vector2D<T>() const { return { static_cast<T>(x), static_cast<T>(y) }; }

	Vector2D<T> up() { return { x , y - 1 }; }
	Vector2D<T> down() { return { x , y + 1 }; }
	Vector2D<T> right() { return { x + 1, y }; }
	Vector2D<T> left() { return { x - 1, y }; }

	Vector2D<T> floor() { return { std::floor(x),std::floor(y) }; }
	Vector2D<T> ceil() { return { std::ceil(x), std::ceil(y) }; }

	void minc(T c) { x = (std::min)(x, c); y = (std::min)(y, c); }
	void maxc(T c) { x = (std::max)(x, c); y = (std::max)(y, c); }
	void range(Vector2D<T> min, Vector2D<T> max) { x = (std::min)((std::max)(min.x, x), max.x); y = (std::min)((std::max)(min.y, y), max.y); }

	Vector2D<T> xplus(T o) { return { x + o, y }; }
	Vector2D<T> yplus(T o) { return { x , y + o }; }

	void normalize() { T n = norm(); x = x / n; y = y / n; }
	double orientation() { return atan2(y, x); }
	T norm() { return sqrt(x * x + y * y); }
	T dot(Vector2D<T>& v) { return (x * v.x) + (y * v.y); };
	//(this X b) X a
	Vector2D<T> tripleCross(Vector2D<T> b, Vector2D<T> a)
	{
		return
		{
			a.y * (b.x * y - b.y * x),
			a.x * (b.y * x - b.x * y)
		};
	}
	void setPolar(T norm, double angle)
	{
		x = norm * (float)cos(angle);
		y = norm * (float)sin(angle);
	}

	Vector2D operator-() const { return Vector2D(-x, -y); }
	friend std::ostream& operator<<(std::ostream& os, const Vector2D& v) { os << "{" << v.x << "," << v.y << "}"; return os; }
};

typedef Vector2D<double> V2d_d;
typedef Vector2D<float> V2d_f;
typedef Vector2D<int> V2d_i;
typedef Vector2D<int> Coords;
typedef Vector2D<int> v2di;

template<class T> Vector2D<T> operator+(const Vector2D<T>& L, const Vector2D<T>& R) { return Vector2D<T>(L) += R; }
template<class T> Vector2D<T> operator-(const Vector2D<T>& L, const Vector2D<T>& R) { return Vector2D<T>(L) -= R; }
template<class T> Vector2D<T> operator*(const Vector2D<T>& L, const Vector2D<T>& R) { return Vector2D<T>(L) *= R; }
template<class T> Vector2D<T> operator/(const Vector2D<T>& L, const Vector2D<T>& R) { return Vector2D<T>(L) /= R; }

template<class L, class R> Vector2D<L> operator+(const Vector2D<L>& l, R r) { return Vector2D<L>(l) += Vector2D<R>(r, r); }
template<class L, class R> Vector2D<L> operator-(const Vector2D<L>& l, R r) { return Vector2D<L>(l) -= Vector2D<R>(r, r); }
template<class L, class R> Vector2D<L> operator*(const Vector2D<L>& l, R r) { return Vector2D<L>(l) *= Vector2D<R>(r, r); }
template<class L, class R> Vector2D<L> operator/(const Vector2D<L>& l, R r) { return Vector2D<L>(l) /= Vector2D<R>(r, r); }

//IMPORTANT!! these operators don'T really make sense in practice: but they allow vectors to be used in maps
template<class L, class R> bool operator< (const Vector2D<L>& v1, const Vector2D<R>& v2)
{
	if (v1.y < v2.y)
		return true;
	else if (v1.y > v2.y)
		return false;
	else
		return (v1.x < v2.x);
}
template<class L, class R> bool operator> (const Vector2D<L>& v1, const Vector2D<R>& v2)
{
	if (v1.y > v2.y)
		return true;
	else if (v1.y < v2.y)
		return false;
	else
		return (v1.x > v2.x);
}


namespace std
{
	template<typename T> struct hash<Vector2D<T>>
	{
		std::size_t operator()(const Vector2D<T>& s) const noexcept
		{
			std::size_t h1 = std::hash<T>{}(s.x);
			std::size_t h2 = std::hash<T>{}(s.y);
			return h1 ^ (h2 << 1); // or use boost::hash_combine
		}
	};
}