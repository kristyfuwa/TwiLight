#pragma once

class Input
{
public:
	Input();
	Input(const Input& input);
	~Input();

	void Initialize();
	void KeyDown(unsigned int ch);
	void KeyUp(unsigned int ch);
	bool IsKeyDown(unsigned int ch);

private:
	bool	m_keys[256];
};