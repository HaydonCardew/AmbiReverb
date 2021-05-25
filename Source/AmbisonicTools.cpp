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
void BformatToPformat(float** data, const unsigned nSamples, const unsigned int order)
{
    assert(order==1); // keep it simple for now...
    const unsigned int nChannels = numberOfChannels(order);
    float w, x, y, z;
    float* flu = data[0];
    float* frd = data[1];
    float* bld = data[2];
    float* bru = data[3];
    for (int sample = 0; sample < nSamples; ++sample)
    {
        w = *flu;
        x = *frd;
        y = *bld;
        z = *bru;
        *(flu++) = (w+x+y+z)/2;
        *(frd++) = (w+x-y-z)/2;
        *(bld++) = (w-x+y-z)/2;
        *(bru++) = (w-x-y+z)/2;
    }
}

void PformatToBformat(float** data, const unsigned nSamples, const unsigned int order)
{
    // the maths is actually the same...
    BformatToPformat(data, nSamples, order);
}

void BformatToPformat(Audio& data, const unsigned nSamples, const unsigned int order)
{
    const unsigned int nChannels = numberOfChannels(order);
    assert(data.size() >=  nChannels);
    assert(data[0].size() >= nSamples);
    for (int sample = 0; sample < nSamples; ++sample)
    {
        const float w = data[0][sample];
        const float x = data[1][sample];
        const float y = data[2][sample];
        const float z = data[3][sample];
        data[0][sample] = (w+x+y+z)/2;
        data[1][sample] = (w+x-y-z)/2;
        data[2][sample] = (w-x+y-z)/2;
        data[3][sample] = (w-x-y+z)/2;
    }
}

void PformatToBformat(Audio& data, const unsigned nSamples, const unsigned int order)
{
    BformatToPformat(data, nSamples, order);
}

}
