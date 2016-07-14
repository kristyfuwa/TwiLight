#include "TCorePublic.h"



Tvector4::Tvector4():m_fX(0.0f),m_fY(0.0f),m_fZ(0.0f),m_fW(0.0f)
{
}


Tvector4::Tvector4(float x, float y, float z, float w):m_fX(x),m_fY(y),m_fZ(z),m_fW(w)
{

}

Tvector4::~Tvector4()
{
}

void Tvector4::Set(float x, float y, float z, float w)
{
	m_fX = x;
	m_fY = y;
	m_fZ = z;
	m_fW = w;
}

float Tvector4::Length() const
{
	return sqrtf(m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ + m_fW*m_fW);
}

float Tvector4::Distance(const Tvector4& vec) const
{
	return sqrtf((vec.m_fX - m_fX)*(vec.m_fX - m_fX) + (vec.m_fY - m_fY)*(vec.m_fY - m_fY) + (vec.m_fZ - m_fZ)*(vec.m_fZ - m_fZ) + (vec.m_fW - m_fW)*(vec.m_fW - m_fW));
}

Tvector4& Tvector4::Normalize()
{
	float value = (m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ + m_fW*m_fW);
	float length = 1.0f / sqrtf(value);
	
	m_fX *= length;
	m_fY *= length;
	m_fZ *= length;
	m_fW *= length;

	return *this;

}

float Tvector4::Dot(const Tvector4& vec) const
{
	return (m_fX*vec.m_fX + m_fY*vec.m_fY + m_fZ*vec.m_fZ + m_fW*vec.m_fW);
}

bool Tvector4::Equal(const Tvector4& vec, float e) const
{
	return fabs(m_fX - vec.m_fX) < e && fabs(m_fY - vec.m_fY) < e &&
		fabs(m_fZ - vec.m_fZ) < e && fabs(m_fW - vec.m_fW);
}

Tvector4 Tvector4::operator-() const
{
	return Tvector4(-m_fX, -m_fY, -m_fZ, -m_fW);
}

Tvector4 Tvector4::operator-(const Tvector4& rhs) const
{
	return Tvector4(m_fX - rhs.m_fX, m_fY - rhs.m_fY, m_fZ - rhs.m_fZ, m_fW - rhs.m_fW);
}

bool Tvector4::operator==(const Tvector4& rhs) const
{
	return (m_fX == rhs.m_fX) && (m_fY == rhs.m_fY) &&
		(m_fZ == rhs.m_fZ) && (m_fW == rhs.m_fW);
}

bool Tvector4::operator!=(const Tvector4& rhs) const
{
	return (m_fX != rhs.m_fX) || (m_fY != rhs.m_fY) ||
		(m_fZ != rhs.m_fZ) || (m_fW != rhs.m_fW);
}

bool Tvector4::operator<(const Tvector4& rhs) const
{
	if (m_fX < rhs.m_fX)
		return true;
	if (m_fX > rhs.m_fX)
		return false;
	if (m_fY < rhs.m_fY)
		return true;
	if (m_fY > rhs.m_fY)
		return false;
	if (m_fZ < rhs.m_fZ)
		return true;
	if (m_fZ > rhs.m_fZ)
		return false;
	if (m_fW < rhs.m_fW)
		return true;
	if (m_fW > rhs.m_fW)
		return false;
	return false;
}

float Tvector4::operator[](int index) const
{
	return (&m_fX)[index];
}

float& Tvector4::operator[](int index)
{
	return (&m_fX)[index];
}

Tvector4 Tvector4::operator*(const Tvector4& rhs) const
{
	return Tvector4(m_fX*rhs.m_fX, m_fY*rhs.m_fY, m_fZ*rhs.m_fZ, m_fW*rhs.m_fW);
}

Tvector4 Tvector4::operator/(const float scale) const
{
	return Tvector4(m_fX / scale, m_fY / scale, m_fZ / scale, m_fW / scale);
}

Tvector4& Tvector4::operator/=(const float scale)
{
	m_fX /= scale;
	m_fY /= scale;
	m_fZ /= scale;
	m_fW /= scale;
	return *this;
}

Tvector4& Tvector4::operator*=(const Tvector4& rhs)
{
	m_fX *= rhs.m_fX;
	m_fY *= rhs.m_fY;
	m_fZ *= rhs.m_fZ;
	m_fW *= rhs.m_fW;
	return *this;
}

Tvector4& Tvector4::operator*=(const float scale)
{
	m_fX *= scale;
	m_fY *= scale;
	m_fZ *= scale;
	m_fW *= scale;
	return *this;
}

Tvector4& Tvector4::operator-=(const Tvector4& rhs)
{
	m_fX -= rhs.m_fX;
	m_fY -= rhs.m_fY;
	m_fZ -= rhs.m_fZ;
	m_fW -= rhs.m_fW;
	return *this;
}

Tvector4& Tvector4::operator+=(const Tvector4& rhs)
{
	m_fX += rhs.m_fX;
	m_fY += rhs.m_fY;
	m_fZ += rhs.m_fZ;
	m_fW += rhs.m_fW;
	return *this;
}

Tvector4 Tvector4::operator+(const Tvector4& rhs) const
{
	return Tvector4(m_fX + rhs.m_fX, m_fY + rhs.m_fY, m_fZ + rhs.m_fZ, m_fW + rhs.m_fW);
}

Tvector4 operator*(const float a, const Tvector4 vec)
{
	return Tvector4(a*vec.m_fX, a*vec.m_fY, a*vec.m_fZ, a*vec.m_fW);
}

std::ostream& operator<<(std::ostream& os, const Tvector4& vec)
{
	os << "(" << vec.m_fX << "," << vec.m_fY << "," << vec.m_fZ << "," << vec.m_fW << ")";
	return os;
}