/*
  ==============================================================================

    AmbisonicTools.cpp
    Created: 24 May 2021 11:09:16pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include <vector>

using namespace std;

namespace Ambisonic
{

enum class Format
{
    FuMa,
    ACN
};

typedef vector<vector<float>> Audio;

unsigned int numberOfChannels(const unsigned int order)
{
    return pow(order-1, 2);
}

/* assume pformat is:
 1 -> FLU
 2 -> FRD
 3 -> BLD
 4 -> BRU
*/
/*
 FLU = W + X + Y + X / 2
 FRD = W + X - Y - X / 2
 BLD = W - X + Y - X / 2
 BRU = W - X - Y + X / 2
 */
// https://www.brucewiggins.co.uk/wp-content/uploads/2017/07/01%20Wiggins%20-%20Ambisonic%20Convolution%20Reverbs.pdf
void BformatToPformat(float** data, const unsigned nSamples, const unsigned int order, Format format)
{
    assert(order==1); // keep it simple for now...
    const unsigned int nChannels = numberOfChannels(order);
    float flu, frd, bld, bru;
    float* w = data[0];
    float* x = data[1];
    float* y = data[2];
    float* z = data[3];
    if (format == Format::ACN)
    {
        y = data[1];
        z = data[2];
        x = data[3];
    }
    for (int sample = 0; sample < nSamples; ++sample)
    {
        flu = *w;
        frd = *x;
        bld = *y;
        bru = *z;
        *(w++) = (flu+frd+bld+bru)/2;
        *(x++) = (flu+frd-bld-bru)/2;
        *(y++) = (flu-frd+bld-bru)/2;
        *(z++) = (flu-frd-bld+bru)/2;
    }
}

void PformatToBformat(float** data, const unsigned nSamples, const unsigned int order, Format format)
{
    // the maths is actually the same...
    BformatToPformat(data, nSamples, order, format);
}

void BformatToPformat(Audio& data, const unsigned nSamples, const unsigned int order, Format format)
{
    const unsigned int nChannels = numberOfChannels(order);
    assert(data.size() >=  nChannels);
    assert(data[0].size() >= nSamples);
    for (int sample = 0; sample < nSamples; ++sample)
    {
        float w = data[0][sample];
        float x = data[1][sample];
        float y = data[2][sample];
        float z = data[3][sample];
        if (format == Format::ACN)
        {
            y = data[1][sample];
            z = data[2][sample];
            x = data[3][sample];
        }
        data[0][sample] = (w+x+y+z)/2;
        data[1][sample] = (w+x-y-z)/2;
        data[2][sample] = (w-x+y-z)/2;
        data[3][sample] = (w-x-y+z)/2;
    }
}

void PformatToBformat(Audio& data, const unsigned nSamples, const unsigned int order, Format format)
{
    BformatToPformat(data, nSamples, order, format);
}

}
