

//
//
//
//#include <iostream>
//#include <chrono>
//#include <time.h>
//#include <iomanip>
//#include <variant>
//#include <torch/torch.h>
////#include <opencv2/opencv.hpp>
////#include <opencv2/imgproc/imgproc.hpp>
//
//
//using namespace std;
////using namespace std::chrono;
//
//#include <iostream>			
//#include <string>			
//#include<variant>
//#include<vector>
//#include<cassert>			
//#include<cmath>
//using namespace std;
//
//void test_base() {
//	// 定义支持int、float、string的类型
//	std::variant<int, float, std::string> x;
//
//	//获取修改值：
//	x = 1; assert(std::get<int>(x) == 1 && std::get<0>(x) == 1);
//	x = 3.14f; assert(std::get<float>(x) == 3.14f && std::get<1>(x) == 3.14f);
//	x = "Tom"; assert(std::get<string>(x) == string("Tom"));
//	x.emplace<string>("Bob"); assert(std::get<2>(x) == string("Bob"));
//
//	//获取变量的指针：
//	x = 10; auto px = std::get_if<int>(&x); cout << *px << endl;//10
//
//	//获取变量索引：
//	x = 1; assert(x.index() == 0);
//	x = 3.14f; assert(x.index() == 1);
//	x = "Tom"; assert(x.index() == 2);
//
//	//判断变量是否有值：
//	cout << holds_alternative<string>(x) << endl;//true 有字符串值
//	cout << holds_alternative<int>(x) << endl;   //false 无int值
//}
//
//
//
//
//
//
//struct Net : torch::nn::Module {
//	Net()
//		: conv1(torch::nn::Conv2dOptions(1, 6, /*kernel_size=*/5).stride(1).padding(0)),
//		pool1(torch::nn::AvgPool2dOptions({ 2, 2 }).stride({ 2, 2 }).padding(0)),
//		conv2(torch::nn::Conv2dOptions(6, 16, /*kernel_size=*/5).stride(1).padding(0)),
//		fc1(16 * 5 * 5, 120),
//		fc2(120, 84),
//		fc3(84, 10) {
//		register_module("conv1", conv1);
//		register_module("pool1", pool1);
//		register_module("conv2", conv2);
//		register_module("fc1", fc1);
//		register_module("fc2", fc2);
//		register_module("fc3", fc3);
//	}
//
//	torch::Tensor forward(torch::Tensor x) {
//		cout << "input: " << x.size(0) << " " << x.size(1) << " " << x.size(2) << " " << x.size(3) << endl;
//		x = torch::relu(conv1->forward(x));
//		cout << "conv1: " << x.size(0) << " " << x.size(1) << " " << x.size(2) << " " << x.size(3) << endl;
//		x = pool1->forward(x);
//		cout << "pool1: " << x.size(0) << " " << x.size(1) << " " << x.size(2) << " " << x.size(3) << endl;
//		x = torch::relu(conv2->forward(x));
//		cout << "conv2: " << x.size(0) << " " << x.size(1) << " " << x.size(2) << " " << x.size(3) << endl;
//		x = pool1->forward(x);
//		cout << "pool2: " << x.size(0) << " " << x.size(1) << " " << x.size(2) << " " << x.size(3) << endl;
//		x = x.view({ x.size(0), -1 });
//		cout << "view: " << x.size(1) << endl;
//		x = torch::relu(fc1->forward(x));
//		cout << "fc1: " << x.size(1) << endl;
//		x = torch::relu(fc2->forward(x));
//		x = torch::softmax(fc3->forward(x), /*dim=*/1);
//		return x;
//	}
//
//	torch::nn::Conv2d conv1;
//	torch::nn::AvgPool2d pool1;
//	torch::nn::Conv2d conv2;
//	torch::nn::Linear fc1;
//	torch::nn::Linear fc2;
//	torch::nn::Linear fc3;
//};
//
//
//
//void testTorch() {
//	// Device
//	torch::DeviceType device_type;
//	if (torch::cuda::is_available())
//	{
//		device_type = torch::kCUDA;
//	}
//	else
//	{
//		device_type = torch::kCPU;
//	}
//
//	torch::Device device(device_type);
//	torch::manual_seed(1234);
//	auto net = std::make_shared<Net>();
//	net->to(device);
//	net->eval();
//	torch::Tensor tmp = torch::ones({ 1, 1, 32, 32 }).to(device);
//	net->train();
//	torch::Tensor out = net->forward(tmp);
//	std::cout << "lenet out shape: " << out.size(1) << std::endl;
//	std::cout << "lenet out: " << out << std::endl;
//	torch::save(net, "lenet5_c.pt");
//
//	std::cout << "train done." << std::endl;
//}
// 
// 
// 



#include <iostream>
#include <chrono>
#include <time.h>
#include <iomanip>

#include <torch/torch.h>
//#include <opencv2/opencv.hpp>
//#include <opencv2/imgproc/imgproc.hpp>


using namespace std;
//using namespace std::chrono;


struct Net : torch::nn::Module {
	Net()
		: conv1(torch::nn::Conv2dOptions(1, 6, /*kernel_size=*/5).stride(1).padding(0)),
		pool1(torch::nn::AvgPool2dOptions({ 2, 2 }).stride({ 2, 2 }).padding(0)),
		conv2(torch::nn::Conv2dOptions(6, 16, /*kernel_size=*/5).stride(1).padding(0)),
		fc1(16 * 5 * 5, 120),
		fc2(120, 84),
		fc3(84, 10) {
		register_module("conv1", conv1);
		register_module("pool1", pool1);
		register_module("conv2", conv2);
		register_module("fc1", fc1);
		register_module("fc2", fc2);
		register_module("fc3", fc3);
	}

	torch::Tensor forward(torch::Tensor x) {
		cout << "input: " << x.size(0) << " " << x.size(1) << " " << x.size(2) << " " << x.size(3) << endl;
		x = torch::relu(conv1->forward(x));
		cout << "conv1: " << x.size(0) << " " << x.size(1) << " " << x.size(2) << " " << x.size(3) << endl;
		x = pool1->forward(x);
		cout << "pool1: " << x.size(0) << " " << x.size(1) << " " << x.size(2) << " " << x.size(3) << endl;
		x = torch::relu(conv2->forward(x));
		cout << "conv2: " << x.size(0) << " " << x.size(1) << " " << x.size(2) << " " << x.size(3) << endl;
		x = pool1->forward(x);
		cout << "pool2: " << x.size(0) << " " << x.size(1) << " " << x.size(2) << " " << x.size(3) << endl;
		x = x.view({ x.size(0), -1 });
		cout << "view: " << x.size(1) << endl;
		x = torch::relu(fc1->forward(x));
		cout << "fc1: " << x.size(1) << endl;
		x = torch::relu(fc2->forward(x));
		x = torch::softmax(fc3->forward(x), /*dim=*/1);
		return x;
	}

	torch::nn::Conv2d conv1;
	torch::nn::AvgPool2d pool1;
	torch::nn::Conv2d conv2;
	torch::nn::Linear fc1;
	torch::nn::Linear fc2;
	torch::nn::Linear fc3;
};

int main(int argc,
	char** argv)
{
	// Device
	torch::DeviceType device_type;
	if (torch::cuda::is_available())
	{
		device_type = torch::kCUDA;
	}
	else
	{
		device_type = torch::kCPU;
	}

	torch::Device device(device_type);
	torch::manual_seed(1234);
	auto net = std::make_shared<Net>();
	net->to(device);
	net->eval();
	torch::Tensor tmp = torch::ones({ 1, 1, 32, 32 }).to(device);
	net->train();
	torch::Tensor out = net->forward(tmp);
	std::cout << "lenet out shape: " << out.size(1) << std::endl;
	std::cout << "lenet out: " << out << std::endl;
	torch::save(net, "lenet5_c.pt");

	std::cout << "train done." << std::endl;
	return (0);
}
