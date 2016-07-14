#pragma once

class Tvector4
{
public:
	Tvector4();
	Tvector4(float x, float y, float z, float w);
	~Tvector4();

	void			Set(float x, float y, float z, float w);
	float			Length() const;
	float			Distance(const Tvector4& vec)	const;
	Tvector4&		Normalize();
	float			Dot(const Tvector4& vec)	const;
	bool			Equal(const Tvector4& vec, float e)	const;

	Tvector4		operator-()	const;
	Tvector4		operator+(const Tvector4& rhs)	const;
	Tvector4		operator-(const Tvector4& rhs)	const;
	Tvector4		operator*(const Tvector4& rhs)	const;
	Tvector4		operator/(const float scale)	const;
	Tvector4&		operator+=(const Tvector4& rhs);
	Tvector4&		operator-=(const Tvector4& rhs);
	Tvector4&		operator*=(const float scale);
	Tvector4&		operator*=(const Tvector4& rhs);
	Tvector4&		operator/=(const float scale);

	bool			operator==(const Tvector4& rhs) const;
	bool			operator!=(const Tvector4& rhs)	const;
	bool			operator<(const Tvector4& rhs)	const;
	float			operator[](int index)	const;
	float&			operator[](int index);

	friend	Tvector4 operator*(const float a, const Tvector4 vec);
	friend std::ostream& operator<<(std::ostream& os, const Tvector4& vec);

private:
	float			m_fX;
	float			m_fY;
	float			m_fZ;
	float			m_fW;
};

