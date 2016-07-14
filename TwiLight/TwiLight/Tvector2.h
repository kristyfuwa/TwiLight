#pragma once

class Tvector2
{
public:
	Tvector2(); 
	Tvector2(float x, float y);

	void		Set(float x, float y);
	float		Length()const;
	float		Distance(const Tvector2& vec) const;
	Tvector2&	Normalize();
	float		Dot(const Tvector2& vec) const;
	bool		Equal(const Tvector2& vec, float e) const;

	//operators
	Tvector2 operator-() const;
	Tvector2 operator+(const Tvector2& rhs) const;//const 不改变成员变量值，常成员函数；
	Tvector2 operator-(const Tvector2& rhs) const;
	Tvector2& operator+=(const Tvector2& rhs);//返回引用的主要目的是实现连级运算
	Tvector2& operator-=(const Tvector2& rhs);
	Tvector2 operator*(const float scale) const;
	Tvector2 operator*(const Tvector2& rhs) const;
	Tvector2& operator*=(const float scale);
	Tvector2 operator/(const float scale) const;
	Tvector2& operator/=(const float scale);
	bool	operator==(const Tvector2& rhs) const;
	bool	operator!=(const Tvector2& rhs) const;
	bool	operator<(const Tvector2& rhs) const;
	float   operator[](int index) const;
	float&	operator[](int index);

	friend Tvector2 operator*(const float a, const Tvector2 vec);
	friend std::ostream& operator<<(std::ostream& os, const Tvector2& vec);

private:
	float m_fX;
	float m_fY;
};
