

#ifndef NBN_DIVISION_BASE_H
#define NBN_DIVISION_BASE_H

#include "../../../core/problem/solution.h"
#include "../../../core/environment/environment.h"
#include <functional>

namespace ofec {
	class NBN_DivisionBase {

	protected:

		int m_maxSample = 1e5;
		int m_numNewSample = 1e4;
		bool m_flag_multiThread = false;
		bool m_flag_evaluate_multitask = false;
	
		int m_numThread = 0;

		std::shared_ptr<Random> m_random;
		std::shared_ptr<ofec::Environment> m_environment;
		int m_dim = -1;
		std::function<void(SolutionBase& sol, Environment *env)> m_eval_fun;
		bool m_flag_addOpt = false;
		

	protected:

	public:
		NBN_DivisionBase() = default;
		virtual	~NBN_DivisionBase() = default;

		virtual size_t size()const  = 0;

		void setEvaluateMultiTask(bool flag) {
			m_flag_evaluate_multitask = flag;
		}
		void setMultiThread(bool flag) {
			m_flag_multiThread = flag;
		}
		void setMaxSampleSize(int sampleSize) {
			m_maxSample = sampleSize;
		}
		void setNewSampleSize(int numNewSampleSize) {
			m_numNewSample = numNewSampleSize;
		}

		void setNumberThread(int numThread) {
			m_numThread = numThread;
		}

		void setOptAdded(bool flag) {
			m_flag_addOpt = flag;
		}


		void setProDim(int dim) {
			m_dim = dim;
		}

		void initialize(
		const std::shared_ptr<Environment>& env,
		const std::shared_ptr<Random> &rnd,
			const std::function<void(SolutionBase& sol, Environment *env)>& eval_fun,
			bool flag_grid_sample = true) {
			m_environment = env;
			m_random = rnd;
			m_eval_fun = eval_fun;
			//initialize_(flag_grid_sample);
			m_dim = m_environment->problem()->numberVariables();
	/*		if (m_flag_addOpt) {
				addOpt();
			}*/
		}


		virtual void initialize_(bool flag_grid_sample = true) = 0;

		virtual void addOpt() {
			int id(0);
			auto optBase(m_environment->problem()->optimaBase());
			if (optBase != nullptr) {
				for (size_t idx(0); idx < optBase->numberSolutions(); ++idx) {
					std::shared_ptr<SolutionBase> cur_sol = nullptr;
					cur_sol.reset(m_environment->problem()->createSolution(optBase->solutionBase(idx).variableBase()));
					//cur_sol.reset(new SolutionType(num_objs, num_cons, num_vars));
					//cur_sol->variable() = dynamic_cast<const SolutionType&>(optBase.variable(idx)).variable();
					m_eval_fun(*cur_sol, m_environment.get());
					addSol(*cur_sol, id, true);
				}
			}
		}
		virtual void addSol(const SolutionBase& new_sol, int& belong_id, bool flag_opt = false,
			int popIter = -1, int popSolId = -1, int algId = -1) = 0;

		virtual const SolutionBase* getSol(int solId) const {
			return nullptr;
		}

		virtual void getSharedNBN(
			std::vector<std::shared_ptr<SolutionBase>>& sols,
			std::vector<double>& fitness,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			std::vector<bool>& flagOpt
		)const = 0;


		virtual void getSharedNBN(
			std::vector<std::shared_ptr<SolutionBase>>& sols,
			std::vector<double>& fitness,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			std::vector<int> popIters,
			std::vector<int> popSolIds,
			std::vector<int> algIds
		)const = 0; 




	};
}

#endif