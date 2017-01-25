#pragma once
namespace Scaled
{
	class Generator
	{
		static int originalPermutation[256];
		int perm[512];

		int mapHeight;
		int mapWidth;
		int octaves;
		float persistence;
		float amplitude;

		float perlinGradientFunc(int hash, float x, float y);
		float perlinLerp(float a, float b, float x);
		float perlinFade(float t);
		float generatePerlin(float x, float y);


	public:
		Generator(int height, int width);
		float Perlin(float x, float y);
		void SetNoiseOctaves(int oct);
		void SetNoisePersistence(float pers);
		void SetAmplitude(float amp);

		float** GenerateMap();
	};
}