
#include "../utility/catch.hpp"

#include <Eigen/Core>
#include <iostream>
#include <random>
#include <vector>

#include "../utility/prediction/mathtoolbox/rbf-interpolation.hpp"
//#include "../utility/Eigen/Core/.."




using Eigen::MatrixXd;
using Eigen::Vector2d;
using Eigen::VectorXd;

namespace
{
    std::random_device                     seed;
    std::default_random_engine             engine(seed());
    std::uniform_real_distribution<double> uniform_dist(-1.0, 1.0);

    double CalcFunction(const Vector2d& x) { return std::sin(10.0 * x(0)) + std::sin(10.0 * x(1)); }
} // namespace

void PerformTest()
{
    constexpr int    number_of_samples = 500;
    constexpr int    number_of_test_samples = 100;
    constexpr double noise_intensity = 0.1;

    // Generate scattered data (in this case, 500 data points in a 2-dimensional space)
    Eigen::MatrixXd X = Eigen::MatrixXd::Random(2, number_of_samples);
    Eigen::VectorXd y(number_of_samples);
    for (int i = 0; i < number_of_samples; ++i)
    {
        y(i) = CalcFunction(X.col(i)) + noise_intensity * uniform_dist(engine);
    }

    // Define interpolation settings
    const auto     kernel = mathtoolbox::ThinPlateSplineRbfKernel();
    constexpr bool use_regularization = true;

    // Instantiate an interpolator
    mathtoolbox::RbfInterpolator rbf_interpolator(kernel);

    // Set data
    rbf_interpolator.SetData(X, y);

    // Calculate internal weights with or without regularization
    rbf_interpolator.CalcWeights(use_regularization);

    // Calculate interpolated values on randomly sampled points
    Eigen::MatrixXd X_test = Eigen::MatrixXd::Random(2, number_of_test_samples);
    Eigen::VectorXd y_test(number_of_test_samples);
  
    for (int i = 0; i < number_of_test_samples; ++i)
    {
        y_test(i) = rbf_interpolator.CalcValue(X_test.col(i));
    }

    // Display the results in the CSV format
    std::cout << "x(0),x(1),y" << std::endl;
    for (int i = 0; i < number_of_test_samples; ++i)
    {
        std::cout << X_test(0, i) << "," << X_test(1, i) << "," << y_test(i) << std::endl;
    }
}


TEST_CASE("UTILITY-MATHTOOLBOX-RBF", "[rbf]")
{
    PerformTest();
}


