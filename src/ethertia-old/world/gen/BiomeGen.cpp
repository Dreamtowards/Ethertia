//
// Created by Dreamtowards on 2023/1/13.
//

#include <ethertia/world/gen/NoiseGen.h>
#include <ethertia/world/Biome.h>

#include <ethertia/util/BitmapImage.h>
#include <ethertia/util/Loader.h>


int main() {

    Biome::find(1,2);


    int seed = 12;

    const int SIZE = 1024;
    static float temp[SIZE*SIZE];
    static float rain[SIZE*SIZE];

    NoiseGen::Perlin()->GenUniformGrid2D(temp, 0, 0, SIZE, SIZE, 0.1, seed);

    BitmapImage* img = new BitmapImage(SIZE, SIZE);

    for (int x = 0; x < SIZE; ++x) {
        for (int z = 0; z < SIZE; ++z) {


            int rgba = 0;
            img->setPixel(x, z, rgba);
        }
    }
    Loader::savePNG(img, "bio.png");

    return 0;
}