#ifndef OFEC_LSTM_H
#define OFEC_LSTM_H


#ifdef  OFEC_DEMO_LITTORCH



#undef slots
#include "torch/torch.h"
#define slots Q_SLOTS

namespace ofec {

	extern torch::Device g_device;
	class LstmLinearImpl : public torch::nn::Module {
	private:
		int64_t m_input_size, m_output_size, m_hidden_size, m_num_layers;
		torch::nn::LSTM m_lstm;
		torch::nn::Linear m_linear;



	public:
		LstmLinearImpl(int64_t input_size, int64_t output_size, int64_t hidden_size, int64_t num_layers);
		torch::Tensor forward(torch::Tensor input);
		int64_t inputSize() const { return m_input_size; }
		int64_t outputSize() const { return m_output_size; }
	};
	TORCH_MODULE(LstmLinear);
}

#endif // !OFEC_LSTM_H


#endif //  OFEC_DEMO_LITTORCH
