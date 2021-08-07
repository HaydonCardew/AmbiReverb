#include "PFormatConfig.h"

PFormatConfigs::PFormatConfigs()
{
    constructConfigs();
}

void PFormatConfigs::constructConfigs()
{
    PFormatConfig T_Design_4("T Design (4)",
    {
        {45,  35.264},
        {135, -35.264},
        {-45, -35.264},
        {-135, 35.264}
    },
    {
        {1.2084e-01,   1.7461e-01,   1.2084e-01,   1.7461e-01},
        {-1.6055e-01,   2.6898e-02,   1.6055e-01,  -2.6898e-02},
        {-2.0422e-01,   1.5614e-01,   2.0422e-01,  -1.5614e-01},
        {-8.5655e-02,   1.6966e-01,  -8.5655e-02,   1.6966e-01},
        {1.6863e-01,  -6.0673e-02,  -1.6863e-01,   6.0673e-02},
        {1.6105e-01,  -5.5348e-02,   1.6105e-01,  -5.5348e-02},
        {1.5965e-02,   2.3068e-02,   1.5965e-02,   2.3068e-02},
        {8.8969e-02,   1.7887e-01,  -8.8969e-02,  -1.7887e-01},
        {-7.1147e-02,   1.5458e-01,  -7.1147e-02,   1.5458e-01},
        {-2.3173e-03,  -1.4600e-02,   2.3173e-03,   1.4600e-02},
        {-1.3497e-01,  -7.7716e-03,  -1.3497e-01,  -7.7716e-03},
        {-1.0884e-01,   1.8235e-02,   1.0884e-01,  -1.8235e-02},
        {9.1180e-02,  -6.9714e-02,  -9.1180e-02,   6.9714e-02},
        {-5.8068e-02,   1.1502e-01,  -5.8068e-02,   1.1502e-01},
        {5.6180e-02,   1.5473e-01,  -5.6180e-02,  -1.5473e-01},
        {4.2229e-02,   5.8274e-02,   4.2229e-02,   5.8274e-02}
    }
    );
    configs.push_back(T_Design_4);
    
    PFormatConfig T_Design_12("T Design (12)",
    {
        {0.0, -31.717},
        {-58.283, 0.0},
        {-90.0, 58.283},
        {0.0, 31.717},
        {-121.717, 0.0},
        {90.0, -58.283},
        {180.0, -31.717},
        {121.717, 0.0},
        {90.0, 58.283},
        {180.0, 31.717},
        {58.283, 0.0},
        {-90.0, -58.283}
    },
    {
        {9.2070e-02, 2.3005e-01, 2.4063e-02, 9.2070e-02, 5.2990e-01, 1.7187e-01, -3.0502e-01, 5.1715e-02, 1.7187e-01, -3.0502e-01, 1.2295e-01, 2.4063e-02},
       {-7.8599e-03, -4.2757e-01, 4.3364e-01, -7.8599e-03, -7.1785e-01, -4.5912e-01, 4.4074e-01, 9.4062e-02, -4.5912e-01, 4.4074e-01, 2.0988e-01, 4.3364e-01},
       {-1.4332e-01, 2.6491e-17, 2.5849e-01, 1.4332e-01, -3.3347e-16, -1.7775e-02, -1.9761e-01, -6.1845e-17, 1.7775e-02, 1.9761e-01, 1.0230e-16, -2.5849e-01},
        {2.0416e-01, 4.5529e-01, -1.3278e-01, 2.0416e-01, 5.7640e-01, 1.7083e-01, -6.2653e-01, -4.0581e-01, 1.7083e-01, -6.2653e-01, 2.3531e-01, -1.3278e-01},
       {-9.9598e-03, -2.8325e-01, 1.0234e-01, -9.9598e-03, -5.6519e-01, -1.3462e-01, 5.5849e-01, -2.2525e-01, -1.3462e-01, 5.5849e-01, 7.3970e-03, 1.0234e-01},
        {1.0249e-02, -5.2402e-16, 6.5275e-01, -1.0249e-02, -7.3998e-16, 6.5660e-01, 6.1735e-02, -1.8392e-16, -6.5660e-01, -6.1735e-02, 1.3306e-16,  -6.5275e-01},
       {-9.7496e-02, -3.6346e-01, 2.9248e-01, -9.7496e-02, -9.8744e-01, -2.0260e-03, 6.0774e-01, -3.4673e-02, -2.0260e-03, 6.0774e-01  -1.5007e-01, 2.9248e-01},
       {-2.5046e-01, 8.5917e-17, -6.8463e-02, 2.5046e-01, -2.2091e-16, -1.0230e-01, 1.4018e-01, 2.0340e-16, 1.0230e-01, -1.4018e-01, 6.1023e-18, 6.8463e-02},
        {1.0518e-01, -2.6525e-01, -3.3454e-02, 1.0518e-01, 4.2844e-01, 4.0310e-02, -1.5222e-01, 1.8980e-01, 4.0310e-02, -1.5222e-01, -3.1870e-01, -3.3454e-02},
       {-9.0143e-03, 9.6785e-02, 9.2629e-02, -9.0143e-03, -9.9487e-01, -1.2184e-01, 5.0547e-01, 2.7946e-01, -1.2184e-01, 5.0547e-01, -3.4645e-01, 9.2629e-02},
       {-6.2832e-02, 3.5473e-17, 3.5524e-01, 6.2832e-02, 1.6178e-16, 3.3162e-01, -3.7846e-01, -4.9683e-16, -3.3162e-01, 3.7846e-01, 2.9155e-16, -3.5524e-01},
        {4.0545e-03, 2.0208e-01, 5.3458e-01, 4.0545e-03, 3.4402e-01, -5.2144e-01, -2.2736e-01, -2.2235e-02, -5.2144e-01, -2.2736e-01, -8.9781e-02, 5.3458e-01},
        {1.8808e-01, 2.4032e-16, 5.9391e-03, -1.8808e-01, 1.0953e-15, -2.3353e-01, 1.8683e-01, 1.5031e-16, 2.3353e-01, -1.8683e-01, 3.0400e-17, -5.9391e-03},
       {-6.4319e-02, 4.5652e-01, -2.6094e-01, -6.4319e-02, 2.1918e+00, 3.8872e-01, -1.3407e+00, 9.0042e-02, 3.8872e-01, -1.3407e+00, -1.4213e-02, -2.6094e-01},
       {-2.8669e-01, 4.6763e-16, -3.6325e-02, 2.8669e-01, 1.2566e-15, -1.6759e-02, 4.3578e-02, -4.6294e-17, 1.6759e-02, -4.3578e-02, 7.8552e-17, 3.6325e-02},
        {5.5907e-02, -5.1226e-01, 2.3564e-01, 5.5907e-02, -1.7488e+00, -3.6800e-01, 1.2457e+00, 2.0413e-01, -3.6800e-01, 1.2457e+00, -7.4878e-02, 2.3564e-01}
    }
    );
    configs.push_back(T_Design_12);
}

vector<string> PFormatConfigs::getConfigsNames() const
{
    vector<string> names(configs.size());
    for (int i = 0; i < configs.size(); ++i)
    {
        names[i] = configs[i].getName();
    }
    return names;
}

int PFormatConfigs::getMaxChannels() const
{
    int maxChannels = 0;
    for (auto & config : configs)
    {
        if (config.getNumChannels() > maxChannels)
        {
            maxChannels = config.getNumChannels();
        }
    }
    return maxChannels;
}

HC::Matrix PFormatConfigs::getDecodingCoefs(const string name) const
{
    for (auto & config : configs)
    {
        if (config.getName() == name)
        {
            return config.getDecodingCoefs();
        }
    }
    assert(false); // this is hairy
    return configs[0].getDecodingCoefs();
}

HC::Matrix PFormatConfigs::getDecodingCoefs(const string name, unsigned ambiOrder) const
{
    const unsigned nChannels = pow(ambiOrder+1, 2);
    HC::Matrix decodingCoefs(getDecodingCoefs(name));
    assert(decodingCoefs.size() >= nChannels);
    while(decodingCoefs.size() > nChannels)
    {
        decodingCoefs.pop_back();
    }
    return decodingCoefs;
}

string PFormatConfig::getName() const
{
    return name;
}

bool PFormatConfig::checkCoefsAreInverse() const
{
    HC::Matrix identity = HC::Matrix::getIdentity(getNumChannels());
    HC::Matrix check = encodingCoefs.multiply(decodingCoefs).subtract(identity);
    // Check should be a matrix of zeros. Allow a tolerance of +/-0.1
    // That's probably enough
    float tolerance = 0.01;
    for (int row = 0; row < check.numRows(); ++row)
    {
        for (int col = 0; col < check.numColumns(); ++col)
        {
            if (abs(check[row][col]) > tolerance)
            {
                return false;
            }
        }
    }
    return true;
}
