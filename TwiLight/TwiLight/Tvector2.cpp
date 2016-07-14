#include "TCorePublic.h"

Tvector2::Tvector2():m_fX(0.0f),m_fY(0.0f)
{

}

Tvector2::Tvector2(float x, float y):m_fX(x),m_fY(y)
{
	
}

void Tvector2::Set(float x, float y)
{
	this->m_fX = x;
	this->m_fY = y;
}

float Tvector2::Length() const
{
	return sqrtf(m_fX*m_fX + m_fY*m_fY);
}

float Tvector2::Distance(const Tvector2& vec) const
{
	Tvector2 temp;
	temp.m_fX = m_fX - vec.m_fX;
	temp.m_fY = m_fY - vec.m_fY;
	return sqrtf(temp.m_fX*temp.m_fX + temp.m_fY*temp.m_fY);
}

Tvector2& Tvector2::Normalize()
{
	float value = m_fX*m_fX + m_fY*m_fY;
	float Length = 1.0f / sqrtf(value);
	m_fX *= Length;
	m_fY *= Length;
	return *this;
}

float Tvector2::Dot(const Tvector2& vec) const
{
	return m_fX*vec.m_fX + m_fY*vec.m_fY;
}

bool Tvector2::Equal(const Tvector2& vec, float e) const
{
	return fabs(m_fX - vec.m_fX) < e && fabs(m_fY - vec.m_fY) < e;
}

Tvector2 Tvector2::operator-() const
{
	return Tvector2(-m_fX, -m_fY);
}

Tvector2 Tvector2::operator+(const Tvector2& rhs) const
{
	return Tvector2(m_fX + rhs.m_fX, m_fY + rhs.m_fY);
}

Tvector2 Tvector2::operator-(const Tvector2& rhs) const
{
	return Tvector2(m_fX - rhs.m_fX, m_fY - rhs.m_fY);
}

Tvector2& Tvector2::operator-=(const Tvector2& rhs)
{
	m_fX -= rhs.m_fX;
	m_fY -= rhs.m_fY;
	return *this;
}

Tvector2& Tvector2::operator+=(const Tvector2& rhs)
{
	m_fX += rhs.m_fX;
	m_fY += rhs.m_fY;
	return *this;
}

float Tvector2::operator[](int index) const
{
	return (&m_fX)[index];
}

float& Tvector2::operator[](int index)
{
	return (&m_fX)[index];
}

Tvector2 Tvector2::operator*(const float scale) const
{
	return Tvector2(m_fX*scale, m_fY*scale);
}

Tvector2 Tvector2::operator*(const Tvector2& rhs) const
{
	return Tvector2(m_fX*rhs.m_fX, m_fY*rhs.m_fY);
}

Tvector2& Tvector2::operator*=(const float scale)
{
	m_fX *= scale;
	m_fY *= scale;
	return *this;
}

Tvector2 Tvector2::operator/(const float scale) const
{

	return Tvector2(m_fX / scale, m_fY / scale);
}

Tvector2& Tvector2::operator/=(const float scale)
{
	m_fX /= scale;
	m_fY /= scale;
	return *this;
}

bool Tvector2::operator<(const Tvector2& rhs) const
{
	if (m_fX < rhs.m_fX)
		return true;
	if (m_fX > rhs.m_fX)
		return false;
	if (m_fY > rhs.m_fY)
		return false;
	if (m_fY < rhs.m_fY)
		return true;
	return false;
}

bool Tvector2::operator!=(const Tvector2& rhs) const
{
	return (m_fX != rhs.m_fX) || (m_fY != rhs.m_fY);
}

bool Tvector2::operator==(const Tvector2& rhs) const
{
	return (m_fX == rhs.m_fX) && (m_fY == rhs.m_fY);
}

Tvector2 operator*(const float a, const Tvector2 vec)
{
	return Tvector2(a*vec.m_fX, a*vec.m_fY);
}

std::ostream& operator<<(std::ostream& os, const Tvector2& vec)
{
	os << "(" << vec.m_fX << "," << vec.m_fY << ")";
	return os;
}