#include "../../utility/catch.hpp"
#include "../../core/global.h"
#include "../../utility/machine_learning/bpnet/lib/Utils.h"
#include "../../utility/machine_learning/bpnet/lib/Net.h"
#include "../../utility/random/newran.h"
#include <iostream>
//#include<qcustomplot.h>

using namespace ofec;

TEST_CASE("BP neural network", "[BPNN]") {
    // Create neural network object
    size_t intput_node = 2;
    size_t hidden_node = 5;
    size_t output_node = 1;
    Real lr = 0.9;
    Real threshold = 1e-4;
    size_t max_epoch = 1e5;
    Random rnd(0.5);
    Net net(intput_node, hidden_node, output_node, lr, threshold, max_epoch, &rnd);

    // Read training data
    std::string training_path = g_working_dir + "/utility/machine_learning/bpnet/data/traindata.txt";
    const std::vector<Sample> trainDataSet = Utils::getTrainData(training_path,2,1);

    // Training neural network
    net.train(trainDataSet);

    // Prediction of samples using neural network
    std::string test_path = g_working_dir + "/utility/machine_learning/bpnet/data/testdata.txt";
    std::vector<Sample> testDataSet = Utils::getTestData(test_path,2);
    std::vector<Sample> predSet = net.predict(testDataSet);
    for (auto& pred : predSet) {
        pred.display();
    }
}

TEST_CASE("BPNN for MOP", "[BPMOP]") {
    // Create neural network object
    size_t intput_node = 2;
    size_t hidden_node = 3;
    size_t output_node = 2;
    Real lr = 0.8;
    Real threshold = 1e-4;
    size_t max_epoch = 1e5;
    Random rnd(0.5);
    Net net(intput_node, hidden_node, output_node,lr,threshold,max_epoch, &rnd);

    // Read training data
    std::string feature_path = g_working_dir + "/instance/problem/continuous/multi_objective/moeadde_f/data/MOP_MOEADDE_F2_var.txt";
    std::string label_path = g_working_dir + "/instance/problem/continuous/multi_objective/moeadde_f/data/MOP_MOEADDE_F2_obj.txt";
    
    std::vector<Real> features = Utils::getFileData(feature_path);
    std::vector < std::vector<Real>> all_features;
    for (size_t i = 0; i < features.size()-1; i += intput_node) {
        std::vector<Real> temp;
        for (size_t j = 0; j < intput_node; ++j) {
            temp.push_back(features[i+j]);
        }
        all_features.emplace_back(temp);
    }

    std::vector<Real> labels = Utils::getFileData(label_path);
    std::vector < std::vector<Real>> all_labels;
    for (size_t i = 0; i < labels.size()-1; i += output_node) {
        std::vector<Real> temp;
        for (size_t j = 0; j < output_node; ++j) {
            temp.push_back(labels[i + j]);
        }
        all_labels.emplace_back(temp);
    }

    //select half for training randomly
    std::vector<size_t> inx;
    for (size_t i = 0; i < all_features.size(); ++i) {
        inx.push_back(i);
    }
    rnd.uniform.shuffle(inx.begin(), inx.end());

    std::vector<Sample> trainDataSet;
    for (size_t i = 0; i < inx.size() / 50; ++i) {
        Sample traindata(all_features[inx[i]], all_labels[inx[i]]);
        trainDataSet.emplace_back(traindata);
    }

    std::vector<Sample> testDataSet;
    std::vector<std::vector<Real>> real_objs;
    for (size_t i = inx.size()/50; i < inx.size() / 50+50; ++i) {
        Sample testdata;
        testdata.setSampleFeatures(all_features[inx[i]]);
        testDataSet.emplace_back(testdata);
        real_objs.emplace_back(all_labels[inx[i]]);
    }

    // Training neural network
    net.train(trainDataSet);

    // Prediction of samples using neural network

    std::vector<Sample> predSet = net.predict(testDataSet);

    for (size_t i = 0;i<predSet.size();++i) {
        for (size_t j = 0; j < output_node; ++j) {
            std::cout << predSet[i].getSampleLabel(j) << "      " << real_objs[i][j] << "     ";
        }
        std::cout<<std::endl;
    }
}

TEST_CASE("auto encoder", "[AE]") {
    // Create neural network object
    size_t intput_node = 2;
    size_t hidden_node = 2;
    size_t output_node = 2;
    Real lr = 0.8;
    Real threshold = 1e-4;
    size_t max_epoch = 1e5;
    Random rnd(0.5);
    Net net(intput_node, hidden_node, output_node, lr, threshold, max_epoch, &rnd);

    // Read training data
    std::string feature_path = g_working_dir + "/instance/problem/continuous/multi_objective/moeadde_f/data/MOP_MOEADDE_F2_var.txt";
    //std::string feature_path = g_working_dir + "/instance/problem/continuous/multi_objective/moeadde_f/data/MOP_MOEADDE_F2_obj.txt";

    std::vector<Real> features = Utils::getFileData(feature_path);
    std::vector < std::vector<Real>> all_features;
    for (size_t i = 0; i < features.size() - 1; i += intput_node) {
        std::vector<Real> temp;
        for (size_t j = 0; j < intput_node; ++j) {
            temp.push_back(features[i + j]);
        }
        all_features.emplace_back(temp);
    }

    std::vector < std::vector<Real>> all_labels= all_features;

    //select half for training randomly
    std::vector<size_t> inx;
    for (size_t i = 0; i < all_features.size(); ++i) {
        inx.push_back(i);
    }
    rnd.uniform.shuffle(inx.begin(), inx.end());

    std::vector<Sample> trainDataSet;
    for (size_t i = 0; i < inx.size() / 50; ++i) {
        Sample traindata(all_features[inx[i]], all_labels[inx[i]]);
        trainDataSet.emplace_back(traindata);
    }

    std::vector<Sample> testDataSet;
    std::vector<std::vector<Real>> real_objs;
    for (size_t i = inx.size() / 50; i < inx.size() / 50 + 10; ++i) {
        Sample testdata;
        testdata.setSampleFeatures(all_features[inx[i]]);
        testDataSet.emplace_back(testdata);
        real_objs.emplace_back(all_labels[inx[i]]);
    }

    // Training neural network
    net.train(trainDataSet);

    // Prediction of samples using neural network

    std::vector<Sample> predSet = net.predict(testDataSet);

    for (size_t i = 0; i < predSet.size(); ++i) {
        for (size_t j = 0; j < output_node; ++j) {
            std::cout << predSet[i].getSampleLabel(j) << "      " << real_objs[i][j] << "     ";
        }
        std::cout << std::endl;
    }
}