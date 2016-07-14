#pragma once

class Tvector3
{
public:
	Tvector3();
	Tvector3(float x, float y, float z);
	~Tvector3();

	void		Set(float x, float y, float z);
	float		Length() const;
	float		Distance(const Tvector3& vec) const;
	Tvector3&	Normalize();
	float		Dot(const Tvector3& vec) const;
	Tvector3	Cross(const Tvector3& vec) const;
	bool		Equal(const Tvector3& vec, float e) const;

	Tvector3	operator-()	const;
	Tvector3	operator+(const Tvector3& rhs)  const;
	Tvector3	operator-(const Tvector3& rhs)	const;
	Tvector3&	operator+=(const Tvector3& rhs);
	Tvector3&	operator-=(const Tvector3& rhs);
	Tvector3	operator*(const float scale)	const;
	Tvector3	operator*(const Tvector3& rhs)	const;
	Tvector3&	operator*=(const float scale);
	Tvector3&	operator*=(const Tvector3& rhs);
	Tvector3	operator/(const float scale)	const;
	Tvector3&	operator/=(const float scale);
	bool		operator==(const Tvector3& rhs) const;
	bool		operator!=(const Tvector3& rhs)	const;
	bool		operator<(const Tvector3& rhs)	const;
	float		operator[](int index)	const;
	float&		operator[](int index);

	friend Tvector3 operator*(const float a, const Tvector3 vec);
	friend std::ostream& operator<<(std::ostream& os, const Tvector3& vec);

private:
	float		m_fX;
	float		m_fY;
	float		m_fZ;
};

