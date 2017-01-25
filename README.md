#### WIP

## ScaledC

A Perlin Noise based terrain generator. Made for self learning. Output is given below:

![Image of ScaledC](https://github.com/vasumahesh1/scaledc/blob/master/Screenshots/screenshot.PNG)


### Usage

```cpp
auto generator = Scaled::Generator(rowLength, columnLength);
generator.SetNoiseOctaves(8);
generator.SetNoisePersistence(0.7f);
generator.SetAmplitude(512);

auto map = generator.GenerateMap();
```