#ifndef NEAREST_BETTER_NETWORK_INFO_H
#define NEAREST_BETTER_NETWORK_INFO_H



#include "../../../core/parameter/variants_to_stream.h"
#include "../../hnsw/hnsw_mt/hnsw_model.h"
#include "../nbn_calculator/nearest_better_calculator.h"
#include "../visualization/tree_graph_simple.h"
#include "../nbn_calculator/nbn_modifiy_equal.h"
#include <set>


namespace ofec {
	namespace nbn {

		struct NBNinfo {

			std::vector<std::shared_ptr<ofec::SolutionBase>> m_sols;
			std::vector<ofec::SolutionBase*> m_solbases;
			std::vector<int> m_solIds;
			std::vector<int> m_belong;
			std::vector<Real> m_dis2parent;
			std::vector<Real> m_vFitness;


			void clear() {
				m_solIds.clear();
				m_solbases.clear();
				m_sols.clear();
				m_belong.clear();
				m_dis2parent.clear();
				m_vFitness.clear();
			}

			void copy(const NBNinfo& ref) {
				m_sols.clear();
				m_solIds = ref.m_solIds;
				m_solbases = ref.m_solbases;
				m_belong = ref.m_belong;
				m_dis2parent = ref.m_dis2parent;
				m_vFitness = ref.m_vFitness;
			}


			void subset(const NBNinfo& ref, const std::vector<int>& selected) {
				clear();
				std::set<int> uniqueIds;
				for (auto& it : selected) {
					uniqueIds.insert(it);
					//lkh_failTrait.push_back(it2);
				}
				std::vector<int> ids;

				for (auto& it : uniqueIds) {
					ids.push_back(it);
				}
				std::sort(ids.begin(), ids.end());
				m_solIds = ids;
				for (auto& it : ids) {
					if (!ref.m_solbases.empty())
						m_solbases.push_back(ref.m_solbases[it]);
					if (!ref.m_belong.empty()) {
						m_belong.push_back(ref.m_belong[it]);
						m_dis2parent.push_back(ref.m_dis2parent[it]);
						m_vFitness.push_back(ref.m_vFitness[it]);
					}
				}
			}
			void add(const NBNinfo& ref) {
				std::vector<int> ids(ref.m_solbases.size());
				for (int idx(0); idx < ids.size(); ++idx) {
					ids[idx] = idx;
				}
				for (auto& it : ids) {
					if (!ref.m_solbases.empty())
						m_solbases.push_back(ref.m_solbases[it]);
					if (!ref.m_belong.empty()) {
						m_belong.push_back(ref.m_belong[it]);
						m_dis2parent.push_back(ref.m_dis2parent[it]);
						m_vFitness.push_back(ref.m_vFitness[it]);
					}
				}
			}


			void initialize(size_t size) {
				m_solIds.resize(size);
				for (int idx(0); idx < size; ++idx) {
					m_solIds[idx] = idx;
				}
				m_belong.resize(size);
				m_dis2parent.resize(size);
				m_vFitness.resize(size);
				std::fill(m_belong.begin(), m_belong.end(), -1);
				std::fill(m_dis2parent.begin(), m_dis2parent.end(), std::numeric_limits<double>::max());
				std::fill(m_vFitness.begin(), m_vFitness.end(), 0);
			}

			void resize(size_t size) {
				m_solIds.resize(size);
				m_belong.resize(size);
				m_dis2parent.resize(size);
				m_vFitness.resize(size);
			}


			void outputNBNinfo(const std::string& saveDir, const std::string& filename) {


				ofec::ParameterVariantStream paramsStream;
				paramsStream << m_belong;
				paramsStream << m_dis2parent;
				paramsStream << m_vFitness;
				auto filepath = saveDir + filename + "_nbnInfo.txt";
				variants_stream::outputToFile(paramsStream, filepath);

			}

			void inputNBNinfo(const std::string& saveDir, const std::string& filename) {
				ofec::ParameterVariantStream paramsStream;
				auto filepath = saveDir + filename + "_nbnInfo.txt";
				variants_stream::inputFromFile(paramsStream, filepath);
				paramsStream >> m_belong;
				paramsStream >> m_dis2parent;
				paramsStream >> m_vFitness;



				m_solIds.resize(m_belong.size());
				for (int idx(0); idx < m_solIds.size(); ++idx) {
					m_solIds[idx] = idx;
				}
			}


			void transferToNBNdata(std::vector<ofec::TreeGraphSimple::NBNdata>& nbn_data) const {
				std::vector<int> solIds(m_belong.size());
				for (int idx(0); idx < solIds.size(); ++idx) {
					solIds[idx] = idx;
				}
				ofec::transferNBNData(nbn_data, solIds, m_belong, m_dis2parent, m_vFitness);

			}



			static inline std::string getSolutionsFileName(const std::string& saveDir, const std::string& filename) {
				return saveDir + filename + "_VecSolutions.txt";
			}


			template<typename T>
			void outputVecSolutions(const std::string& saveDir, const std::string& filename, ofec::Environment* env) {
				auto pro = dynamic_cast<ofec::ProblemVariableVector<T>*>(env->problem());
				ofec::ParameterVariantStream paramsStream;
				paramsStream << m_solbases.size();
				for (auto& it : m_solbases) {
					pro->solutionToParameterVariants(*it, paramsStream);
				}
				//auto filepath = saveDir + filename + "_VecSolutions.txt";

				variants_stream::outputToFile(paramsStream, getSolutionsFileName(saveDir, filename));
			}

			template<typename T>
			void inputVecSolutions(const std::string& saveDir, const std::string& filename, ofec::Environment* env) {
				auto pro = dynamic_cast<ofec::ProblemVariableVector<T>*>(env->problem());
				ofec::ParameterVariantStream paramsStream;
				//	auto filepath = saveDir + filename + "_VecSolutions.txt";
				variants_stream::inputFromFile(paramsStream, getSolutionsFileName(saveDir, filename));

				size_t solSize;
				paramsStream >> solSize;
				m_sols.resize(solSize);
				m_solbases.clear();
				for (auto& it : m_sols) {
					it.reset(pro->createSolution());
					m_solbases.push_back(it.get());

				}
				for (auto& it : m_solbases) {
					pro->parameterVariantsToSolution(paramsStream, *it);
				}
			}



			void fromNBNdata(const std::vector<ofec::TreeGraphSimple::NBNdata>& nbn_data) {
				m_belong.resize(nbn_data.size());
				m_dis2parent.resize(nbn_data.size());
				m_vFitness.resize(nbn_data.size());

				for (int idx(0); idx < nbn_data.size(); ++idx) {
					m_belong[idx] = nbn_data[idx].m_belong;
					m_dis2parent[idx] = nbn_data[idx].m_dis2parent;
					m_vFitness[idx] = nbn_data[idx].m_fitness;
				}
			}


			void toNBNdata(std::vector<ofec::TreeGraphSimple::NBNdata>& nbn_data)const {
				nbn_data.resize(m_belong.size());


				for (int idx(0); idx < nbn_data.size(); ++idx) {
					nbn_data[idx].m_belong = m_belong[idx];
					nbn_data[idx].m_dis2parent = m_dis2parent[idx];
					nbn_data[idx].m_fitness = m_vFitness[idx];
				}
			}


			void trasferToGraph(ofec::TreeGraphSimple& nbn_graph, int maxLength) {
				{

					//	auto filename = tspname + "_randomSamples" + "_neighborK_" + std::to_string(neighborK);
					std::vector<int> solIds(m_belong.size());
					for (int idx(0); idx < solIds.size(); ++idx) {
						solIds[idx] = idx;
					}
					std::vector<ofec::TreeGraphSimple::NBNdata> nbn_data;
					ofec::transferNBNData(nbn_data, solIds, m_belong, m_dis2parent, m_vFitness);

					;
					nbn_graph.setNBNdata(nbn_data);
					//nbn_graph.modifyBestSols(rnd.get(), env->problem(), nbnInfo.solbases);
					nbn_graph.calNetwork(maxLength);


					//std::string savepath = saveDir2 + filename + "_network.txt";
					//std::ofstream out(savepath);
					//nbn_graph.outputNBNnetwork(out);
					//out.close();
					//ouputNBNdata(saveDir2 + filename + "_nbn.txt", nbn_graph.get_nbn_data());
				}
			}
			void modify() {
				if (m_belong.back() == m_belong.size() - 1 && m_vFitness.back() == 0) {
					int rootId = m_belong.size() - 1;
					for (int idx(0); idx < m_belong.size(); ++idx) {
						if (m_belong[idx] == rootId) {
							m_belong[idx] = -1;
						}
					}

					m_belong.pop_back();
					m_dis2parent.pop_back();
					m_vFitness.pop_back();
				}
			}





			void calculateNBNhnsw(ofec::Environment* env, ofec::Random* rnd) {
				auto pro = env->problem();
				auto& solbases = m_solbases;
				hnsw::HnswModel hnswModel;
				hnswModel.initialize(env, rnd, std::thread::hardware_concurrency());
				std::vector<int> solIds;
				int cursolId(0);
				for (auto& it : solbases) {
					hnswModel.addDataSingleThread(it);
					solIds.push_back(cursolId++);
				}


				auto& belong = m_belong;
				auto& dis2parent = m_dis2parent;
				auto& vFitness = m_vFitness;
				vFitness.clear();

				for (auto& it : solbases) {
					vFitness.push_back(it->fitness());
				}


				std::vector<std::vector<int>> model_neighbors(solbases.size());
				for (int idx(0); idx < solbases.size(); ++idx) {
					auto& nei = hnswModel.getNeighbors(idx);
					auto& curnei = model_neighbors[idx];
					for (auto& neiId : nei.neighbors()) {
						curnei.push_back(neiId.nodeId());
					}
				}



				//for (int idx(0); idx < solbases.size(); ++idx) {
				//	auto& nei = model_neighbors[idx][0];
				//	auto& curnei = neighbors[idx];
				//	curnei = nei;
				//}

				{
					//	std::cout << "begin nbn" << std::endl;
					//	auto start = std::chrono::system_clock::now();
					ofec::NBN_NearestBetterCalculator::calculate(solbases, vFitness, model_neighbors,
						belong, dis2parent,
						pro, rnd);
				}
			}


		};
	}
}


#endif
