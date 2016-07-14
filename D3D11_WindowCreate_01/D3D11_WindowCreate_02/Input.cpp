#include "Input.h"

Input::Input()
{

}

Input::Input(const Input& input)
{

}
Input::~Input()
{

}

void Input::Initialize()
{
	for (int i = 0; i < 256; ++i)
	{
		m_keys[i] = false;
	}
	return;
}

void Input::KeyDown(unsigned int ch)
{
	m_keys[ch] = true;
	return;
}
void Input::KeyUp(unsigned int ch)
{
	m_keys[ch] = false;
	return;
}
bool Input::IsKeyDown(unsigned int ch)
{
	return m_keys[ch];
}
