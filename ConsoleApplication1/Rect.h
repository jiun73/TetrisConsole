#pragma once
#include "Vector2D.h"

template<class T>
struct rRectangle 
{
	Vector2D<T> pos;
	Vector2D<T> sz;

	rRectangle() : pos(Vector2D<T>()), sz(Vector2D<T>()) {}
	rRectangle(const T px, const T py, const T sx, const T sy) : pos(px, py), sz(sx, sy) {}
	rRectangle(const T _r) : pos(_r), sz(_r) {}
	rRectangle(const Vector2D<T> _pos, const Vector2D<T> _sz) : pos(_pos), sz(_sz) {}
	rRectangle(const rRectangle& _r) : pos(_r.pos), sz(_r.sz) {}
	~rRectangle() {}

	friend bool operator==(const rRectangle& v1, const rRectangle& v2) { return (v1.pos == v2.pos && v1.sz == v2.sz); }

	friend std::ostream& operator<<(std::ostream& os, const rRectangle& r)
	{
		os << "{[" << r.pos.x << "," << r.pos.y << "][" << r.sz.x << "," << r.sz.y << "]}";
		return os;
	}

	template<class T> operator rRectangle<T>() const { return rRectangle<T>(static_cast<T>(pos.x), static_cast<T>(pos.y), static_cast<T>(sz.x), static_cast<T>(sz.y)); }
};

typedef rRectangle<double> Rect_d;
typedef rRectangle<float> Rect_f;
typedef rRectangle<int> Rect;
