#include "cnn.hpp"

cnn::cnn()
{
}

cnn::~cnn()
{
}

// z[n] = w[n]*a[n-1] + b[n]
// a[n] = g(z[n])

void cnn::convolution(std::vector<uint8_t>& imageData) {
	data.resize(imageData.size());
	data = imageData;
	convolutionMask.resize(3 * 3 * 1);
	convolutionMask = {
		1, 0, -1,
		1, 0, -1,
		1, 0, -1,
	};

	convo = {
	};
}

void cnn::activation() {

}
