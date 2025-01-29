/********* Begin Register Information **********
{ 
	"description": "LSTM from LibTorch",
	"dependency on libraries": [ "LibTorch" ]
}
*********** End Register Information **********/

#include "../catch_amalgamated.hpp"
#include <torch/torch.h>

TEST_CASE("prediction", "[LibTorch][LSTM]") {
	torch::Tensor m_input;
}