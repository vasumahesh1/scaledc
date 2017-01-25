#include "Scaled.h"

#include <stdexcept>

using namespace std;

namespace Scaled
{
	// Hash Lookup defined by Ken Perlin.
	// Randomly arranged array of all numbers from 0-255 inclusive.
	int Generator::originalPermutation[256] = { 151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,    
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	Generator::Generator(int height, int width)
	{
		mapHeight = height;
		mapWidth = width;

		// Repeat the Permutation Array so as to avoid Buffer Overflow later on.
		for (int idx = 0; idx < 512; idx++) {
			perm[idx] = Generator::originalPermutation[idx % 256];
		}
	}

	float Generator::perlinFade(float t) {
		// This eases coordinate values so that they will ease towards integral values.
		return t * t * t * (t * (t * 6 - 15) + 10); // 6t^5 - 15t^4 + 10t^3
	}

	float Generator::perlinGradientFunc(int hash, float x, float y) {
		switch (hash & 0xF)
		{
			case 0x0: return  x;						// Dot Prod with (1, 0)
			case 0x1: return  x + y;					// Dot Prod with (1, 1)
			case 0x2: return  y;						// Dot Prod with (0, 1)
			case 0x3: return -x + y;					// Dot Prod with (-1, 1)
			case 0x4: return -x;						// Dot Prod with (-1, 0)
			case 0x5: return -x - y;					// Dot Prod with (-1,-1)
			case 0x6: return  -y;						// Dot Prod with (0, -1)
			case 0x7: return x - y;						// Dot Prod with (1, -1)
			
			// Repeat
			case 0x8: return  x;
			case 0x9: return  x + y;
			case 0xA: return  y;
			case 0xB: return -x + y;
			case 0xC: return -x;
			case 0xD: return -x - y;
			case 0xE: return  -y;
			case 0xF: return x - y;
			
			default: return 0; // Never occurs
		}
	}

	// Linear Interpolate
	float Generator::perlinLerp(float a, float b, float x) {
		return ((1.0f - x) * a) + (x * b);
	}

	float Generator::generatePerlin(float x, float y)
	{
		// Determine the Integral Cell Coordinates
		int x0 = (int)x & 255;
		int y0 = (int)y & 255;

		int x1 = x0 + 1;
		int y1 = y0 + 1;

		// Determine relative distance from x0, y0
		float sx = x - (float)x0; // Value between 0.0 to 1.0
		float sy = y - (float)y0; // Value between 0.0 to 1.0

		float u = perlinFade(sx);
		float v = perlinFade(sy);

		// Hash Values for determining the Gradient Vector
		int aa, ab, ba, bb;
		aa = perm[perm[x0] + y0];
		ab = perm[perm[x0] + y1];
		ba = perm[perm[x1] + y0];
		bb = perm[perm[x1] + y1];

		float x1f, x2f, n0, n1;

		// Calc Dot Product
		n0 = perlinGradientFunc(aa, sx, sy);
		n1 = perlinGradientFunc(ba, sx - 1, sy);
		// Linear Interpolate
		x1f = perlinLerp(n0, n1, u);

		// Calc Dot Product
		n0 = perlinGradientFunc(ab, sx, sy - 1);
		n1 = perlinGradientFunc(bb, sx - 1, sy - 1);
		// Linear Interpolate
		x2f = perlinLerp(n0, n1, u);

		auto value = (perlinLerp(x1f, x2f, v) + 1) / 2;

		return value;
	}

	void Generator::SetNoiseOctaves(int oct) {
		octaves = oct;
	}

	void Generator::SetNoisePersistence(float pers) {
		persistence = pers;
	}

	void Generator::SetAmplitude(float amp) {
		amplitude = amp;
	}

	float Generator::Perlin(float x, float y) {
		float total = 0;
		float frequency = 1;
		float maxValue = 0;  // Used for normalizing result to 0.0 - 1.0

		for (int i = 0; i< octaves; i++) {
			total = total + (generatePerlin(x * frequency, y * frequency) * amplitude);

			maxValue += amplitude;

			amplitude *= persistence;
			frequency *= 2;
		}

		return total / maxValue;
	}

	float** Generator::GenerateMap()
	{
		float** map = new float*[mapWidth];

		for (int i = 0; i < mapWidth; i++) {
			map[i] = new float[mapHeight];
			for (int j = 0; j < mapHeight; j++) {
				map[i][j] = 0;
			}
		}

		for (int i = 0; i < mapWidth; i++) {
			for (int j = 0; j < mapHeight; j++) {
				float nx = ((float)i / mapWidth) + 0.5f;
				float ny = ((float)j / mapHeight) + 0.5f;
				auto value = Perlin(nx, ny);
				map[i][j] += value;
			}
		}

		return map;
	}
}