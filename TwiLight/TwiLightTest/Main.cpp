#include "TCorePublic.h"

#pragma comment(lib,"TwiLight.lib")

int main()
{
	Tvector2 vec(2.0f, 3.0f);
	Tvector2 vec2(1.0f, 4.0f);
	Tvector2 vec3 = vec2 + vec;

	Tvector3 vec4(2.0f, 3.0f,5.0f);
	Tvector3 vec5(1.0f, 4.0f,5.0f);
	Tvector3 vec6 = vec4 + vec5;

	float value = vec6[1];

	float scale = vec3[2];

	return 0;
}
