
#include <torch/torch.h>
#include <iostream>
int main()
{
    torch::DeviceType device_type;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA available! Predicting on GPU." << std::endl;
        device_type = torch::kCUDA;
    }
    else {
        std::cout << "Predicting on CPU." << std::endl;
        device_type = torch::kCPU;
    }
    torch::Device device(device_type);
    torch::Tensor tensor = torch::eye(3);
    tensor = tensor.to(at::kCUDA);
    // std:cout<<tensor<<std::endl;
    std::cout << "hello torch" << std::endl;
    std::cout << tensor << std::endl;
    return 0;
}

