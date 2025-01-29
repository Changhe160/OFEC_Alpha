/********* Begin Register Information **********
{
	"name": "CSIWDN",
	"identifier": "CSIWDN",
	"tags": [ "contamination source identification for water distribution network" ]
}
*********** End Register Information **********/
#ifndef OFEC_CSIWDN_H
#define OFEC_CSIWDN_H                                                          

#include "../../../../core/problem/optima.h"
#include "../../../../core/problem/problem.h"
#include "../../../../core/problem/solution.h"
#include "csiwdn_encoding.h"
#include <string>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sstream>
#include <fstream>
#include <exception>

#include "epanet/epanet.h"
#include "q_ahc.h"
#include "k_means.h"
#include <mutex>


#ifdef  OFEC_LITTORCH
#include "../../../../utility/machine_learning/libtorch/model/lstm.h"
#include "torch/torch.h"
#endif

#define CAST_CSIWDN(pro) dynamic_cast<CSIWDN*>(pro)
namespace ofec {
	/* Contamination Source Identification for Water Distribution Network */
	class CSIWDN : public Problem {
		OFEC_CONCRETE_INSTANCE(CSIWDN)
	public:	

		enum class InitType { kCluster, kRandom, kDistance, kBeVisited };	

		enum class ClusterType {kAHC, kKMeans, kNOCluster};
	
		enum  class DistanceType { Mix, Integer, Continous};

		using solutionType = ofec::Solution<VarCSIWDN>;
	
		struct Pipe {
			std::string pipe_id;
			std::string sNode;
			std::string eNode;
			double length;
			double diameter;
			std::vector<float> flow_rate;
		};

		struct Pipe_node {
			int m_node;
			double m_weight;
		};

		struct PathInfo {
			int m_start, m_end;
			double m_length;
			friend bool operator<(const PathInfo &a, const PathInfo &b) {
				return a.m_length > b.m_length;
			}
		};

	protected:

		static int m_totalCswidnPro;
		static std::mutex m_totalPro_mtx;
		

		epanet::EPANET m_epanet;

		//static std::mutex ms_mutex_epanet;

		int m_phase = 1;
		VarCSIWDN m_real_PS_read;

		Real m_objective_accuracy;

		std::vector<std::vector<float>> m_Ciobs;	// Real value
		int m_num_source;							// number of source    
		int m_num_sensor;							// number of sensors
		int m_num_node;								 // number of nodes
		long m_quality_time_step;					// time step of recording quality 
		int m_num_quality_records_each_phase;		// number of quality records of each phase
		int m_total_phase;							// number of phases (evaluation function changes when phase increases)
		int m_first_phase;                          // the phase when contamination detected by any sensor
	
		long m_min_duration, m_max_duration;
		long m_total_duration;
		float m_min_multiplier, m_max_multiplier;
		std::vector<long> m_inject_duration;

		long m_min_start_time, m_max_start_time;

		/* for simplification of problem */
		long m_pattern_step;
		int m_num_pattern;   // number of pattern : m_total_duration / m_pattern_step

		std::vector<std::string> m_sensor_Loc;          // location of sensors , first time detected
		std::vector<int> m_sensor_nodeId;

		std::string m_full_path;
		std::string m_map_name, m_sensor_case, m_source_case;                // file name of input data  
		//std::string m_fileRnd = "";
		std::stringstream m_map_inp;
		std::stringstream m_map_rpt;

		std::stringstream m_map_rpt_bn;
		
		
		std::stringstream m_hyd;

		std::vector<int> m_pattern_index;
		InitType m_init_type = InitType::kCluster;
		ClusterType m_cluster_type = ClusterType::kAHC;

		std::vector<bool> m_index_flag;
		bool m_start_algorithm;

		std::vector<Real> m_distance_node;  //compute distance vaule of nodes, for initialize by distance 

		std::vector<std::string> m_node_name;
		std::map<std::string, int> m_node_id;
		std::vector<std::vector<Real>> m_location;
		std::vector<std::vector<bool>> m_adjmatrix;
		std::vector<std::vector<bool>> m_directed_adjmatrix;


		size_t m_pop_identifier;
		csiwdn::Q_AHC m_q_ahc;
		csiwdn::KMeans m_k_means; 
		std::vector<std::vector<size_t>> m_clusters;

		std::vector<std::vector<int>> m_num_be_visited;
		std::vector<std::vector<double>> m_pro_be_visited;

		std::vector<Pipe> m_pipe;
		std::vector<std::vector<float>> m_node_quality;

		std::vector<std::vector<std::vector<int>>> m_shortest_pathes;

		// calculate the distance between two solution
		double m_dis_ratio = 0.5;
		double m_max_location_dis;


		// distance calculator type
		DistanceType m_distanceType;
		std::pair<int, int> m_source_idx;
		

		std::string m_processName;

	public:


		void setDistanceType(DistanceType type) {
			m_distanceType = type;
		}
		const DistanceType& distanceType()const {
			return m_distanceType;
		}
		void setSourceIdx(const std::pair<int, int>& sourceIdx) {
			m_source_idx = sourceIdx;
		}
		std::pair<int, int> sourceIdx()const {
			return m_source_idx;
		}
		//void setFileRnd(const std::string& filenRnd) {
		//	m_fileRnd = filenRnd;
		//}

		void setProcessName(const std::string& processName) {
			m_processName = processName;
		}

		const std::string& processName()const {
			return m_processName;
		}
	protected:

		void initialize_(Environment* env) override;
		void updateOptima(Environment* env) override;

		void addInputParameters();
		//void evaluate_(SolutionBase &s) override;

		virtual void evaluate(const VariableBase& vars, std::vector<Real>& objs, std::vector<Real>& cons)const override;




		void evaluateObjective(VarCSIWDN &x, std::vector<Real> &obj)const;
		void readSensorParams();
		void readSourceParams();

		void readLocation();
		
		void updateAdjmatrix();
		void updateShortestPathes();
		void readConnect();

		bool m_use_lstm_model;

	public:
		void useLstmModel(bool flag) { m_use_lstm_model = flag; }
		bool isUseLstmModel() { return m_use_lstm_model; }
#ifdef  OFEC_LITTORCH

    protected:
		/* Libtorch prediction */

		std::vector<std::unique_ptr<LstmLinear>> m_lstm_model;
		bool m_lstm_model_initialized;
		int m_input_size, m_output_size;
		std::vector<std::pair<float, float>> m_input_range, m_output_range;

		void initializeLSTM();
		void getCitByLSTM(VarCSIWDN& sol, std::vector<std::vector<float>>& data);

#endif

	public:

		//virtual void updateOptBase() override;

		virtual SolutionBase* createSolution() const override {
			return new Solution<VarCSIWDN>(numberObjectives(), numberConstraints(), m_num_source);
			auto it = new Solution<VarCSIWDN>(numberObjectives(), numberConstraints(), m_num_source);
			//	it->variable() = m_optima()->solution(0).variable();
			return it;
		};
		virtual SolutionBase* createSolution(const SolutionBase& s)const override {
			auto it = new Solution<VarCSIWDN>(dynamic_cast<const Solution<VarCSIWDN>&>(s));
			//it->variable() = m_optima()->solution(0).variable();
			return it;
		}
		virtual SolutionBase* createSolution(const VariableBase& s) const override {
			auto curSol = new Solution<VarCSIWDN>(numberObjectives(), numberConstraints(), m_num_source);
			curSol->variable() = dynamic_cast<const VarCSIWDN&>(s);
			return curSol;
		}

		virtual VariableBase* createVariables() const override {
			return new VarCSIWDN(m_num_source);
		}
		virtual VariableBase* createVariables(const VariableBase& x) const override {
			return new VarCSIWDN(m_num_source);
		}
		
		void setDeaultVaribleValue(VariableBase& x)const;
		


		void initializeVariables(VariableBase& x, Random* rnd) const override;
		//void initializeSolution(SolutionBase& s, Random* rnd) const override;

		void initSolutionPosition(SolutionBase& s, Random* rnd, int num);
		void setSolutionPosition(SolutionBase& s, const SolutionBase& copy);
		
		void initSolutionMultiplier(SolutionBase& s, Random* rnd);

		void initSolutionMultiplier(SolutionBase& s, const std::vector<std::vector<std::pair<double,double>>>&range, Random* rnd);
		void setSolutionMultiplier(SolutionBase& s, const SolutionBase& copy);
		

		// for output to file 
		virtual void solutionToParameterVariants(const SolutionBase& sol, ParameterVariantStream& out)const override;
		virtual void parameterVariantsToSolution(ParameterVariantStream& in, SolutionBase& sol) const override;

		//virtual ~CSIWDN();

		int compareSols(const VariableBase& x1, const VariableBase& x2) const ;
		bool same(const VariableBase &x1, const VariableBase &x2) const override;
		void fitlerSameSolutions(std::vector<ofec::SolutionBase*>& sols,
			std::vector<ofec::SolutionBase*>& filterSols, std::vector<int>& originIds);

		//defined by DYY
	//	Real variableDistance(const SolutionBase& s1, const SolutionBase& s2) const override;
		Real variableDistance(const VariableBase& s1, const VariableBase& s2) const override;
		Real variableDistance(const VariableBase& s1, const VariableBase& s2, const std::pair<int, int>& source_idx) const ;


		virtual Real normalizedVariableDistance(const VariableBase& s1, const VariableBase& s2) const override{
			return variableDistance(s1, s2);
			return variableDistance(s1, s2) / (m_num_source+1);
		}

		virtual Real maximumVariableDistance()const {
			return 1.0;
			return m_num_source + 1;
		}
		const Optima<VarCSIWDN>* optima() const { return dynamic_cast<Optima<VarCSIWDN>*>(m_optima.get()); }
		//void updateCandidates(const SolutionBase &sol, std::list<std::unique_ptr<SolutionBase>> &candidates) const override;

		size_t popIdentifier() const { return  m_pop_identifier; }
		void setPopIdentifier(size_t i) { m_pop_identifier = i; }
		int phase() const { return m_phase; }

		void setPhase(int num) { m_phase = num; }
	//	void phaseNext() { if (m_phase < m_total_phase) ++m_phase; };
		void updatePhase(int phase);
		int firstPhase() { return m_first_phase; }
		int interval() const { return m_num_quality_records_each_phase; }
		int totalPhase() const { return m_total_phase; }
		long totalRunTime() const { return m_total_duration; }
		const std::vector<Real> &distanceNode() const { return m_distance_node; }
		std::vector<Real> &distanceNode() { return m_distance_node; }
		int numSensor() const { return m_num_sensor; }
		int numSource() const { return m_num_source; }
		const std::vector<std::vector<float>> &observationConcentration() const { return m_Ciobs; }
		const std::vector<std::vector<float>> &nodeQuality() const { return m_node_quality; }
		const std::map<std::string, int> &nodeId() const { return m_node_id; }
		bool isTimeOut() const;
		int endSourceIdx() const;
		int numberNode() const { return m_num_node; }
		int numberSource() const { return m_num_source; }
		long patternStep() const { return m_pattern_step; }
		bool indexFlag(size_t i) const { return m_index_flag[i]; }
		long qualityTimeStep() const { return m_quality_time_step; }
		long intervalTimeStep() const { return m_quality_time_step * m_num_quality_records_each_phase; }
		int minStartTime() const { return m_min_start_time; }
		int maxStartTimeSize()const { return (m_max_start_time - m_min_start_time) / m_pattern_step + 1; }
		int minDuration() const { return m_min_duration; }
		int maxDurationSize() const { return (m_max_duration - m_min_duration) / m_pattern_step + 1; }
		float minMultiplier() const { return m_min_multiplier; }
		float maxMultiplier() const { return m_max_multiplier; }
		const std::vector<Real> &getCoordinates(int idx) const { return m_location[idx - 1]; }
		const std::vector<std::vector<Real>> &getCoordinates()const { return m_location; }
		const std::vector<std::vector<bool>> &getAdjmatrix() const { return m_adjmatrix; }
		const std::vector<std::vector<bool>>& getDirectedAdjmatrix() const { return m_directed_adjmatrix; }
		const std::vector<int>& sensorIds()const {
			return m_sensor_nodeId;
		}
		const std::vector<std::string>& nodeName() const {
			return m_node_name;
		}
		void setInitType(InitType type) { m_init_type = type; }
		InitType initType() const { return m_init_type; }
		void setClusterType(ClusterType type) { m_cluster_type = type; }
		ClusterType clusterType() const { return m_cluster_type; }
		long injectDuration(size_t i) const { return m_inject_duration[i]; }
		void clustering(size_t num, Random *rnd);
		const std::vector<std::vector<size_t>>& clusters() const { return m_clusters; }

		bool isAlgorithmStarted() const { return m_start_algorithm; }
		void setAlgorithmStart() { m_start_algorithm = true; }
		float calculateDistance(int index1, int index2) const;
		void calProByBeVisited();
		const std::vector<std::vector<double>>& getProBeVisited() const { return m_pro_be_visited; }

		csiwdn::Q_AHC& get_Q_AHC() { return m_q_ahc; }


		void getCitByEPANET(epanet::EPANET& epanet,VarCSIWDN &sol, std::vector<std::vector<float>> &data)const;
		void getEPANETinfo();
		void initializeEPANET(epanet::EPANET& epanet)const;



		const std::vector<int>& shortestPath(int start, int end) const { return m_shortest_pathes[start][end]; }
		const std::vector<std::vector<int>>& shortestPath(int start) const { return m_shortest_pathes[start]; }
		const std::vector<size_t>& currentCluster() const { return m_clusters[m_pop_identifier]; }

		std::string mapName() const { return m_map_name; }
		std::string sensorCase() const { return m_sensor_case; }
		std::string sourceCase() const { return m_source_case; }


		void outputSol(std::ostream& out, ofec::SolutionBase& sol)const;
		void inputSol(std::istream& in, ofec::SolutionBase& sol)const;
		bool inputSol(std::stringstream& in, ofec::SolutionBase& sol)const;
		bool inputSol(FILE* fp, ofec::SolutionBase& sol)const;

		//void setUnfeasibleSol()
};
}


#endif // !OFEC_CSIWDN_H



