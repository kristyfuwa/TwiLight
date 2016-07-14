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
	Tvector2 operator+(const Tvector2& rhs) const;//const ���ı��Ա����ֵ������Ա������
	Tvector2 operator-(const Tvector2& rhs) const;
	Tvector2& operator+=(const Tvector2& rhs);//�������õ���ҪĿ����ʵ����������
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
