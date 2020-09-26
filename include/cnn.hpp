#ifndef _CNN_HPP__
#define _CNN_HPP__

#include <iostream>
#include <vector>
class cnn
{
public:
	cnn();
	~cnn();

private:
	std::vector<uint8_t> data;
	std::vector<int8_t> convo;
	std::vector<int8_t> convolutionMask;
	std::vector<uint8_t> weights;
	std::vector<uint8_t> bias;
	std::vector<uint8_t> act;
	std::vector<uint8_t> theta;


	void convolution(std::vector<uint8_t>& imageData);
	void activation();
};

#endif // !_CNN_HPP_
