#include "../../utility/catch.hpp"
#include "../../core/global.h"
#include "../../utility/random/newran.h"
#include "../../utility/clustering/kmeans.h"
#include <iostream>
//#include<qcustomplot.h>

using namespace ofec;

TEST_CASE("kmeans clustering", "[Kmeans]") {
	size_t total_points = 7;
	size_t dims = 2;
	size_t K = 2;
	size_t max_iterations = 100;
	size_t has_name=0;
	Random rnd(0.5);
	/*std::cout << "please input:  total points    dims    K   max_iterations   if_has_name" << std::endl;
	std::cin >> total_points >> dims >> K >> max_iterations >> has_name;*/

	std::vector<KmeansPoint> points;
	std::string point_name;

	std::vector<Real> dim1 = { 1.0,1.5,3.0,5.0,3.5,4.5,3.5 };
	std::vector<Real> dim2 = { 1.0,2.0,4.0,7.0,5.0,5.0,4.5 };

	std::vector<std::vector<Real>> all_data;
	//values=[[1.0 1.0],[1.5 2.0],[3.0 4.0] [5.0 7.0],[3.5 5.0],[4.5 5.0],[3.5 4.5]];
	for (int i = 0; i < total_points; i++){
		std::vector<Real> values;
		values.push_back(dim1[i]);
		values.push_back(dim2[i]);
		all_data.emplace_back(values);
		if (has_name){
			std::cin >> point_name;
			KmeansPoint p1(i, values, point_name);
			points.push_back(p1);
		}
		else{
			KmeansPoint p(i, values);
			points.push_back(p);
		}
	}

	KMeans kmeans(K, total_points, dims, max_iterations);
	//kmeans.run(points,&rnd);
	kmeans.run(all_data, &rnd);

    //show clusterings
	kmeans.showClusters();
}