#include "TCorePublic.h"

Tvector3::Tvector3():m_fX(0.0f),m_fY(0.0f),m_fZ(0.0f)
{
}


Tvector3::Tvector3(float x, float y, float z):m_fX(x),m_fY(y),m_fZ(z)
{

}

Tvector3::~Tvector3()
{
}

void Tvector3::Set(float x, float y, float z)
{
	m_fX = x;
	m_fY = y;
	m_fZ = z;
}

float Tvector3::Length() const
{
	return sqrtf(m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ);
}

float Tvector3::Distance(const Tvector3& vec) const
{
	return sqrtf((m_fX - vec.m_fX)*(m_fX - vec.m_fX) + (m_fY - vec.m_fY)*(m_fY - vec.m_fY) + (m_fZ - vec.m_fZ)*(m_fZ - vec.m_fZ));
}

Tvector3& Tvector3::Normalize()
{
	float value = m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ;

	float length = 1.0f / sqrtf(value);

	m_fX *= length;
	m_fY *= length; 
	m_fZ *= length;
	return *this;
}

float Tvector3::Dot(const Tvector3& vec) const
{
	return m_fX*vec.m_fX + m_fY*vec.m_fY + m_fZ*vec.m_fZ;
}

Tvector3 Tvector3::Cross(const Tvector3& vec) const
{
	return Tvector3(m_fY*vec.m_fZ - m_fZ*vec.m_fY, m_fZ*vec.m_fX - m_fX*vec.m_fZ, m_fX*vec.m_fY - m_fY*vec.m_fX);
}

bool Tvector3::Equal(const Tvector3& vec, float e) const
{
	return fabs(m_fX - vec.m_fX) < e && fabs(m_fY - vec.m_fY) < e && fabs(m_fZ - vec.m_fZ) < e;
}

Tvector3 Tvector3::operator-() const
{
	return Tvector3(-m_fX, -m_fY, -m_fZ);
}

Tvector3 Tvector3::operator-(const Tvector3& rhs) const
{
	return Tvector3(m_fX - rhs.m_fX, m_fY - rhs.m_fY, m_fZ - rhs.m_fZ);
}

Tvector3& Tvector3::operator-=(const Tvector3& rhs)
{
	m_fX -= rhs.m_fX;
	m_fY -= rhs.m_fY;
	m_fZ -= rhs.m_fZ;
	return *this;
}

bool Tvector3::operator<(const Tvector3& rhs) const
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
	return false;
}

float Tvector3::operator[](int index) const
{
	return (&m_fX)[index];
}

float& Tvector3::operator[](int index)
{
	return (&m_fX)[index];
}

Tvector3 Tvector3::operator*(const float scale) const
{
	return Tvector3(m_fX*scale, m_fY*scale, m_fZ*scale);
}

Tvector3 Tvector3::operator*(const Tvector3& rhs) const
{
	return Tvector3(m_fX*rhs.m_fX, m_fY*rhs.m_fY, m_fZ*rhs.m_fZ);
}

Tvector3& Tvector3::operator*=(const float scale)
{
	m_fX *= scale;
	m_fY *= scale;
	m_fZ *= scale;
	return *this;
}

Tvector3& Tvector3::operator*=(const Tvector3& rhs)
{
	m_fX *= rhs.m_fX;
	m_fY *= rhs.m_fY;
	m_fZ *= rhs.m_fZ;
	return *this;
}

Tvector3 Tvector3::operator/(const float scale) const
{
	return Tvector3(m_fX / scale, m_fY / scale, m_fZ / scale);
	
}

Tvector3& Tvector3::operator/=(const float scale)
{
	m_fX /= scale;
	m_fY /= scale;
	m_fZ /= scale;
	return *this;
}

bool Tvector3::operator!=(const Tvector3& rhs) const
{
	return (m_fX != rhs.m_fX) || (m_fY != rhs.m_fY) || (m_fZ != rhs.m_fZ);
}

bool Tvector3::operator==(const Tvector3& rhs) const
{
	return (m_fX == rhs.m_fX) && (m_fY == rhs.m_fY) && (m_fZ == rhs.m_fZ);
}

Tvector3& Tvector3::operator+=(const Tvector3& rhs)
{
	m_fX += rhs.m_fX;
	m_fY += rhs.m_fY;
	m_fZ += rhs.m_fZ;
	return *this;
}

Tvector3 Tvector3::operator+(const Tvector3& rhs) const
{
	return Tvector3(m_fX + rhs.m_fX, m_fY + rhs.m_fY, m_fZ + rhs.m_fZ);
}

Tvector3 operator*(const float a, const Tvector3 vec)
{
	return Tvector3(a*vec.m_fX, a*vec.m_fY, a*vec.m_fZ);
}

std::ostream& operator<<(std::ostream& os, const Tvector3& vec)
{
	os << "(" << vec.m_fX << "," << vec.m_fY << "," << vec.m_fZ << ")";
	return os;
}
