#pragma once
#include <glm/gtc/noise.hpp>
#define PI 3.14159265359
class HeightsGenerator
{
public:

	float AMPLITUDE = 70.0f;
	float seed;

	HeightsGenerator()
	{
		seed = rand() % 1000000000;
	}

	float generateHeight(int x, int z)
	{
		float total = 0.0;
		total += 1 * getNoise(x, z);
		total += 0.5 * getNoise(x/2.0f, z/2.0f);
		total += 0.25 * getNoise(x / 4.0f, z/ 4.0f);
		total += 4 * getNoise(x / 6.0f, z/6.0f);
		total += 4 * getNoise(x / 1.2f, z / 1.2f);

		//total = pow(total, 0.5);
		return total;
	}

	float interpolate(float a, float b, float blend)
	{
		double theta = blend * PI;
		float f = (float)(1.0f - cos(theta)) * 0.5f;
		return a * (1.0f - f) + b * f;
	}

	float getInterpolatedNoise(float x, float z)
	{
		int intX = (int)x;
		int intZ = (int)z;

		float fracX = x - intX;
		float fracZ = z - intZ;

		float v1 = getSmoothNoise(intX, intZ);
		float v2 = getSmoothNoise(intX + 1, intZ);
		float v3 = getSmoothNoise(intX, intZ + 1);
		float v4 = getSmoothNoise(intX + 1, intZ + 1);
		float i1 = interpolate(v1, v2, fracX);
		float i2 = interpolate(v3, v4, fracX);

		return interpolate(i1, i2, fracZ);

	}

	float getSmoothNoise(int x, int z)
	{
		float corners = (getNoise(x - 1, z - 1) + getNoise(x + 1, z - 1) + getNoise(x - 1, z + 1) + getNoise(x + 1, z + 1)) / 16.0f;
		float sides = (getNoise(x - 1, z) + getNoise(x + 1, z) + getNoise(x, z - 1) + getNoise(x, z + 1)) / 8.0f;
		float center = getNoise(x, z) / 4.0f;

		return corners + sides + center;
	}


	
	float getNoise(int x, int z)
	{
		srand(x * 47855 + z * 26744 + seed);
		//float noise = (double)rand() / RAND_MAX * 2.0f -1.0f ;
		float noise = glm::simplex(glm::vec2(x/16.0f, z/16.0f));
	
		noise *= 5;
		return noise;
	}
};
