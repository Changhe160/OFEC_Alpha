/********* Begin Register Information **********
{
    "dependency on libraries": [ "Eigen" ]
}
*********** End Register Information **********/

#include "../catch_amalgamated.hpp"
#include "../../core/global.h"
#include "../../utility/machine_learning/autoencoder/utils.h"
#include "../../utility/machine_learning/autoencoder/softmax.h"
#include "../../utility/machine_learning/autoencoder/DAE.h"
#include "../../utility/machine_learning/autoencoder/stackedAE.h"
#include "../../utility/random/newran.h"
#include <iostream>

#include <fstream>
#include <string>
#include "../../utility/machine_learning/autoencoder/Read_Data.h"
#include "../../utility/machine_learning/autoencoder/Cifar_Loader.h"
#include <Eigen/Core>
#include <Eigen/Dense>
//#include<qcustomplot.h>

using namespace ofec;

TEST_CASE("auto encoder", "[AE]") {
    int ae1HiddenSize = 100;
    int ae2HiddenSize = 100;
    int numClasses = 10;
    double noiseRatio[2] = { 0.3,0.3 };
    double lambda[4] = { 3e-3,3e-3,3e-3,1e-4 };
    double alpha[4] = { 0.2,0.2,0.2,0.2 };
    double beta[2] = { 3,3 };
    int maxIter[4] = { 100,100,100,200 };
    int miniBatchSize[4] = { 1000,1000,1000,1000 };
    Eigen::MatrixXd trainingData(1, 1);
    Eigen::MatrixXi trainingLabels(1, 1);
    Eigen::MatrixXd testData(1, 1);
    Eigen::MatrixXi testLabels(1, 1);
    clock_t start = clock();
    bool mnist = true;
    if (mnist) {
        bool ret;
        ret = loadMnistData(trainingData, "/Users/ruizhang/Desktop/hw5/denosing_autoencoder/mnist/train-images-idx3-ubyte");
        std::cout << trainingData.rows() << " " << trainingData.cols() << std::endl;
        std::cout << "Loading training data..." << std::endl;
        ret = loadMnistLabels(trainingLabels, "/Users/ruizhang/Desktop/hw5/denosing_autoencoder/mnist/train-labels-idx1-ubyte");
        ret = loadMnistData(testData, "/Users/ruizhang/Desktop/hw5/denosing_autoencoder/mnist/t10k-images-idx3-ubyte");
        ret = loadMnistLabels(testLabels, "/Users/ruizhang/Desktop/hw5/denosing_autoencoder/mnist/t10k-labels-idx1-ubyte");
        std::ofstream ofs("test.txt", std::ofstream::out);
        ofs << testLabels << std::endl;
        ofs.close();
    }
    else {
        CIFARLoader loader("/Users/ruizhang/Desktop/hw5/test/Cifar");
        trainingData = loader.trainingInput.transpose();
        std::cout << trainingData.rows() << " " << trainingData.cols() << std::endl;
        trainingLabels = loader.trainingOutput;
        testData = loader.testInput.transpose();
        std::cout << testData.rows() << " " << testData.cols() << std::endl;
        testLabels = loader.testOutput;
        std::ofstream ofs("test.txt", std::ofstream::out);
        ofs << testLabels << std::endl;
        ofs.close();
    }

    StackedAE stackedAE(ae1HiddenSize, ae2HiddenSize, numClasses);
    stackedAE.preTrain(trainingData, trainingLabels, lambda, alpha, miniBatchSize,
        maxIter, noiseRatio, beta);
    std::cout << "Loading test data..." << std::endl;
    Eigen::MatrixXi pred1 = stackedAE.predict(testData);
    std::cout << pred1.rows() << " " << pred1.cols() << std::endl;
    std::cout << testLabels.rows() << " " << testLabels.cols() << std::endl;
    double acc1 = calcAccurancy(testLabels, pred1);
    std::cout << "Accurancy before fine tuning: " << acc1 * 100 << "%" << std::endl;
    Eigen::MatrixXd aeTheta1 = stackedAE.getAe1Theta();
    Eigen::MatrixXd aeTheta2 = stackedAE.getAe2Theta();
    Eigen::MatrixXd filter = aeTheta2 * aeTheta1;
    std::cout << "Fine Tuning..." << std::endl;
    stackedAE.fineTune(trainingData, trainingLabels, lambda[3],
        alpha[3], maxIter[3], miniBatchSize[3]);
    Eigen::MatrixXi pred2 = stackedAE.predict(testData);
    double acc2 = calcAccurancy(testLabels, pred2);
    std::cout << "Accurancy: " << acc2 * 100 << "%" << std::endl;
    aeTheta1 = stackedAE.getAe1Theta();
    aeTheta2 = stackedAE.getAe2Theta();
    filter = aeTheta2 * aeTheta1;
    clock_t end = clock();
    std::cout << "The code ran for " <<
        (end - start) / (double)(CLOCKS_PER_SEC * 60) <<
        " minutes on " << Eigen::nbThreads() << " thread(s)." << std::endl;
}