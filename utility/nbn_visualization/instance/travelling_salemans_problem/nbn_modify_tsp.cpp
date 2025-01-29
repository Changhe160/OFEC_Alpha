#include "nbn_modify_tsp.h"
#include "../../../../core/parameter/variants_to_stream.h"
#include "../../../function/custom_function.h"



namespace ofec {
	namespace nbn {
		namespace tsp {
			void outputTspSolutions(const std::string& saveDir, const std::string& filename, std::vector<SolutionBase*>& solbases) {
				ParameterVariantStream paramsStream;
				paramsStream << solbases.size();
				for (auto& it : solbases) {
					auto& cursol = dynamic_cast<ofec::TravellingSalesman::SolutionType&>(*it);
					paramsStream << cursol.variable().vector();
				}
				variants_stream::outputToFile(paramsStream, saveDir + filename + "_solVariables.txt");
				//	outputToFile(paramsStream, saveDir + filename + "_solVariables.txt");

			}



			void sampleSolsAroundTSPThreadTask(
				const ofec::TravellingSalesman::SolutionType& sol,
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int neighbor_k,
				int from, int to,
				ofec::Environment* env,
				ofec::Random* rnd
			) {

				auto pro = env->problem();


				int a(0), b(0);
				int dim = pro->numberVariables();

				std::shared_ptr<ofec::SolutionBase> cursol;
				for (int idx(from); idx < to; ++idx) {

					cursol.reset(new ofec::TravellingSalesman::SolutionType(sol));
					//	cursol.reset(pro->createSolution());

				//	cursol.reset(pro->createSolution(sol));
					auto& cursolt = dynamic_cast<ofec::TravellingSalesman::SolutionType&>(*cursol);
					for (int idy(0); idy < neighbor_k; ++idy) {
						a = rnd->uniform.nextNonStd<int>(0, dim);
						b = rnd->uniform.nextNonStd<int>(0, dim);
						std::swap(cursolt.variable()[a], cursolt.variable()[b]);
					}
					cursol->evaluate(env, false);
					samples[idx] = cursol;

				}
			}



			void sampleSolsAroundTSPMultiThread(
				const ofec::TravellingSalesman::SolutionType& sol,
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int neighbor_k,
				int numSamples,
				ofec::Environment* env,
				ofec::Random* rnd
			) {

				samples.resize(numSamples);


				std::cout << "generate solutions by multithread" << std::endl;
				int num_task = std::thread::hardware_concurrency();
				int num_samples = numSamples;
				std::vector<std::thread> thrds;
				std::vector<int> tasks;
				UTILITY::assignThreads(num_samples, num_task, tasks);


				std::vector<std::shared_ptr<ofec::Random>> rnds(num_task);
				for (auto& it : rnds) {
					double randomSeed(rnd->uniform.nextNonStd<double>(0.01, 1.0));
					it.reset(new ofec::Random(randomSeed));
				}
				for (size_t i = 0; i < num_task; ++i) {

					thrds.push_back(std::thread(
						sampleSolsAroundTSPThreadTask,
						std::cref(sol), std::ref(samples), neighbor_k,
						tasks[i], tasks[i + 1], env, rnds[i].get()));
				}
				for (auto& thrd : thrds)
					thrd.join();
			}


			void kOptSwap(std::vector<int>& path,  ofec::Random* rnd) {
				int n = path.size(); // ·���бߵ���������������
			//	if (K <= 0 || K > n) return; // Kֵ������ʱֱ�ӷ���

				// ���ѡ�������и�㣬��·����Ϊ�������м䲿�ֽ�����ת
				int cut1 = rnd->uniform.nextNonStd<int>(0,n); // ��һ���и��
				int cut2 = cut1 + rnd->uniform.nextNonStd<int>(0, n-cut1); // �ڶ����и�㣬��֤���͵�һ���и���غ�

				// ��ת�� cut1 �� cut2 ֮�����·��
				reverse(path.begin() + cut1, path.begin() + cut2);

				// ������������Ӵ�����������·���������Ƿ�����������
				// ...
			}


			void sampleSolsAroundTSPThreadTask_kSwap(
				const ofec::TravellingSalesman::SolutionType& sol,
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int neighbor_k,
				int from, int to,
				ofec::Environment* env,
				ofec::Random* rnd
			) {

				auto pro = env->problem();


				int a(0), b(0);
				int dim = pro->numberVariables();

				std::shared_ptr<ofec::SolutionBase> cursol;
				for (int idx(from); idx < to; ++idx) {

					cursol.reset(new ofec::TravellingSalesman::SolutionType(sol));
					//	cursol.reset(pro->createSolution());

				//	cursol.reset(pro->createSolution(sol));
					auto& cursolt = dynamic_cast<ofec::TravellingSalesman::SolutionType&>(*cursol);

					auto& cursolvec= cursolt.variable().vector();



					for (int idy(0); idy < neighbor_k; ++idy) {


						kOptSwap(cursolvec, rnd);

						//a = rnd->uniform.nextNonStd<int>(0, dim);
						//b = rnd->uniform.nextNonStd<int>(0, dim);
						//std::swap(cursolt.variable()[a], cursolt.variable()[b]);
					}
					cursol->evaluate(env, false);
					samples[idx] = cursol;

				}
			}



			void sampleSolsAroundTSPMultiThread_kSwap(
				const ofec::TravellingSalesman::SolutionType& sol,
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int neighbor_k,
				int numSamples,
				ofec::Environment* env,
				ofec::Random* rnd
			) {
				samples.resize(numSamples);


				std::cout << "generate solutions by multithread kOpt\t"<< neighbor_k << std::endl;
				int num_task = std::thread::hardware_concurrency();
				int num_samples = numSamples;
				std::vector<std::thread> thrds;
				std::vector<int> tasks;
				UTILITY::assignThreads(num_samples, num_task, tasks);


				std::vector<std::shared_ptr<ofec::Random>> rnds(num_task);
				for (auto& it : rnds) {
					double randomSeed(rnd->uniform.nextNonStd<double>(0.01, 1.0));
					it.reset(new ofec::Random(randomSeed));
				}
				for (size_t i = 0; i < num_task; ++i) {

					thrds.push_back(std::thread(
						sampleSolsAroundTSPThreadTask_kSwap,
						std::cref(sol), std::ref(samples), neighbor_k,
						tasks[i], tasks[i + 1], env, rnds[i].get()));
				}
				for (auto& thrd : thrds)
					thrd.join();
			}



			void sampleSolsAroundTSP_threadTask_subsequenceRND(
				const ofec::TravellingSalesman::SolutionType& sol,
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int neighbor_k,
				int from, int to,
				ofec::Environment* env,
				ofec::Random* rnd
			) {
				auto pro = env->problem();
				int dim = pro->numberVariables();

				std::shared_ptr<ofec::SolutionBase> cursol;

				std::vector<int> shuffleIds(dim);
				for (int idx(0); idx < dim; ++idx) {
					shuffleIds[idx] = idx;
				}

				for (int idx(from); idx < to; ++idx) {
					cursol.reset(new ofec::TravellingSalesman::SolutionType(sol));
					auto& cursolt = dynamic_cast<ofec::TravellingSalesman::SolutionType&>(*cursol);
					auto& cursolvec = cursolt.variable().vector();
					rnd->uniform.shuffle(shuffleIds.begin(), shuffleIds.end());
					std::vector<int> curIds(neighbor_k);
					for (int selectedId(0); selectedId < neighbor_k; ++selectedId) {
						curIds[selectedId] = cursolvec[shuffleIds[selectedId]];
					}
					rnd->uniform.shuffle(curIds.begin(), curIds.end());
					for (int selectedId(0); selectedId < neighbor_k; ++selectedId) {
						cursolvec[shuffleIds[selectedId]] = curIds[selectedId];
					}


					cursol->evaluate(env, false);
					samples[idx] = cursol;
				}
			}

			void sampleSolsAroundTSPMultiThread_subsequenceRND(
				const ofec::TravellingSalesman::SolutionType& sol,
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int neighbor_k,
				int numSamples,
				ofec::Environment* env,
				ofec::Random* rnd
			) {
				samples.resize(numSamples);


				std::cout << "sampleSolsAroundTSPMultiThread_subsequenceRND  by multithread kOpt\t" << neighbor_k << std::endl;
				int num_task = std::thread::hardware_concurrency();
				int num_samples = numSamples;
				std::vector<std::thread> thrds;
				std::vector<int> tasks;
				UTILITY::assignThreads(num_samples, num_task, tasks);


				std::vector<std::shared_ptr<ofec::Random>> rnds(num_task);
				for (auto& it : rnds) {
					double randomSeed(rnd->uniform.nextNonStd<double>(0.01, 1.0));
					it.reset(new ofec::Random(randomSeed));
				}
				for (size_t i = 0; i < num_task; ++i) {

					thrds.push_back(std::thread(
						sampleSolsAroundTSP_threadTask_subsequenceRND,
						std::cref(sol), std::ref(samples), neighbor_k,
						tasks[i], tasks[i + 1], env, rnds[i].get()));
				}
				for (auto& thrd : thrds)
					thrd.join();
			
			}



			void sampleSolsRandomThreadTask(
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int from, int to,
				ofec::Environment* env,
				ofec::Random* rnd
			) {
				auto pro = env->problem();
				int dim = pro->numberVariables();
				//	std::cout << "dim\t" << dim << std::endl;
				std::shared_ptr<ofec::SolutionBase> cursol;
				for (int idx(from); idx < to; ++idx) {
					cursol.reset(new ofec::TravellingSalesman::SolutionType(pro->numberObjectives(), pro->numberConstraints(), dim));
					//	cursol.reset(pro->createSolution());
					cursol->initialize(env, rnd);
					//	auto& cursolt = dynamic_cast<ofec::TravellingSalesman::SolutionType&>(*cursol);
					samples[idx] = cursol;
					cursol->evaluate(env, false);
				}
			}



			void sampleSolsRandomMultiThread(
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int numSamples,
				ofec::Environment* env,
				ofec::Random* rnd
			) {
				samples.resize(numSamples);



				int num_task = std::thread::hardware_concurrency();
				int num_samples = numSamples;
				std::vector<std::thread> thrds;
				std::vector<int> tasks;
				UTILITY::assignThreads(num_samples, num_task, tasks);


				std::vector<std::shared_ptr<ofec::Random>> rnds(num_task);
				for (auto& it : rnds) {
					double randomSeed(rnd->uniform.nextNonStd<double>(0.01, 1.0));
					it.reset(new ofec::Random(randomSeed));
				}

				std::pair<int, int> from_to;
				for (size_t i = 0; i < num_task; ++i) {
					from_to.first = tasks[i];
					from_to.second = tasks[i + 1];

					thrds.push_back(std::thread(
						sampleSolsRandomThreadTask,
						std::ref(samples),
						tasks[i], tasks[i + 1], env, rnds[i].get()));
				}
				for (auto& thrd : thrds)
					thrd.join();
			}




			void filterUniqueSols(const std::vector<ofec::SolutionBase*>& originSol,
				std::vector<ofec::SolutionBase*>& filteredSols,
				std::vector<int>& toSolId,
				ofec::TravellingSalesman* tsp_pro, ofec::Random* rnd
			) {

				ofec::TravellingSalesman::HashSolutionMap solMap;
				solMap.initialize(rnd, tsp_pro->numberVariables() + 10);
				toSolId.resize(originSol.size());
				filteredSols.clear();
				for (int idx(0); idx < originSol.size(); ++idx) {
					toSolId[idx] = solMap.getSolId(*originSol[idx]);
					if (toSolId[idx] == filteredSols.size()) {
						filteredSols.push_back(originSol[idx]);
					}
				}

			}



			void sortEachSolutionXThreadTask(std::vector<ofec::SolutionBase*>& sols,
				int from,int to
			) {
				for (int idx(from); idx < to; ++idx) {
					auto& cursol = sols[idx];
					ofec::TravellingSalesman::sortedSolutionX(*cursol);
				}
			}
			

			void sortEachSolutionX(std::vector<ofec::SolutionBase*>& sols) {
				std::cout << "generate solutions by multithread" << std::endl;
				int num_task = std::thread::hardware_concurrency();
				int num_samples = sols.size();
				std::vector<std::thread> thrds;
				std::vector<int> tasks;
				UTILITY::assignThreads(num_samples, num_task, tasks);
				
				for (size_t i = 0; i < num_task; ++i) {

					thrds.push_back(std::thread(
						sortEachSolutionXThreadTask,
						std::ref(sols), tasks[i], tasks[i + 1]));
				}
				for (auto& thrd : thrds)
					thrd.join();
				
			}
		}
	}
}
