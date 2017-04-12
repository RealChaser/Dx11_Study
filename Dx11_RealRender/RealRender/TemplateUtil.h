#pragma once
#include "stdafx.h"

template <typename T>
void Safe_Delete(T& p)
{
	if (p)
	{
		delete	p;
		p = nullptr;
	}
}

template <typename T>
void Safe_Release(T& p)
{
	if (p)
	{
		while (p->Release()){}
		p = nullptr;
	}
}

template <typename T>
void Safe_Release_VecList(T& p)
{
	T::iterator	iter = p.begin();

	while (iter != p.end())
	{
		Safe_Release(*iter);
		++iter;
	}

	p.clear();
}

template <typename T>
void Safe_Delete_VecList(T& p)
{
	T::iterator	iter = p.begin();

	while (iter != p.end())
	{
		Safe_Delete(*iter);
		++iter;
	}

	p.clear();
}

template <typename T>
void Safe_Delete_Map(T& p)
{
	T::iterator iter = p.begin();
	while (iter != p.end())
	{
		Safe_Delete(iter->second);
		++iter;
	}
	p.clear();
}

template <typename T>
void Safe_Release_Map(T& p)
{
	T::iterator iter = p.begin();
	while (iter != p.end())
	{
		Safe_Release(iter->second);
		++iter;
	}
	p.clear();
}

template <typename T>
void Safe_Delete_ListInMap(T& p)
{
	T::iterator iter = p.begin();
	while (iter != p.end())
	{
		Safe_Delete_VecList(iter->second);
		++iter;
	}
	p.clear();
}

/////////////////////////////////////////////////////////////

template<typename T>
static T Min(const T& a, const T& b)
{
	return a < b ? a : b;
}

template<typename T>
static T Max(const T& a, const T& b)
{
	return a > b ? a : b;
}

template<typename T>
static T Lerp(const T& a, const T& b, float t)
{
	return a + (b - a)*t;
}

template<typename T>
static T Clamp(const T& x, const T& low, const T& high)
{
	return x < low ? low : (x > high ? high : x);
}