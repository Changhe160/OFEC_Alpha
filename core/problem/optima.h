/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao & Junchen Wang & Changhe Li
* Email: diaoyiyacug@gmail.com & wangjunchen.chris@gmail.com & changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class Optima stores a set of optimal solutions for a given problem
*
*********************************************************************************/

#ifndef OFEC_OPTIMA_H
#define OFEC_OPTIMA_H

#include "solution.h"
#include <memory>

namespace ofec {
    class OptimaBase {
    protected:
        std::vector<std::vector<Real>> m_objectives;          // optima with only objective values known

    public:
        OptimaBase() = default;
        OptimaBase(const OptimaBase &rhs) : 
            m_objectives(rhs.m_objectives) {}
        OptimaBase(OptimaBase &&rhs) noexcept = default;
        virtual ~OptimaBase() = default;
        OptimaBase& operator=(const OptimaBase &rhs);
        OptimaBase& operator=(OptimaBase &&rhs) noexcept = default;

        virtual bool isSolutionGiven() const = 0;
        virtual size_t numberSolutions() const = 0;
        virtual const SolutionBase& solutionBase(size_t i) const = 0;

        virtual bool isObjectiveGiven() const { return !m_objectives.empty(); }
        virtual size_t numberObjectives() const { return m_objectives.size(); }
        virtual const std::vector<Real>& objective(size_t i) const { return m_objectives[i]; }

        void appendObjective(const std::vector<Real> &obj) { m_objectives.push_back(obj); }
        void appendObjective(Real obj) { m_objectives.push_back({ obj }); }

        virtual void clear() { m_objectives.clear(); }
    };

    template<typename TVariable = VariableVector<Real>>
    class Optima : public OptimaBase {
    protected:
        std::vector<Solution<TVariable>> m_solutions;   // optima with entire solution known

    public:
        Optima() = default;
        Optima(const Optima &rhs) : OptimaBase(rhs) {
            for (size_t i = 0; i < rhs.m_solutions.size(); ++i) {
                m_solutions.emplace_back(rhs.m_solutions[i]);
            }
        }
        Optima(Optima &&rhs) noexcept = default;
        ~Optima() {}
        Optima& operator=(const Optima &rhs) {
            if (this != &rhs) {
                OptimaBase::operator=(rhs);
                m_solutions.clear();
                for (size_t i = 0; i < rhs.m_solutions.size(); ++i) {
                    m_solutions.emplace_back(rhs.m_solutions[i]);
                }
            }
            return *this;
        }
        Optima& operator=(Optima &&rhs) noexcept = default;

        bool isSolutionGiven() const override { return !m_solutions.empty(); }
        size_t numberSolutions() const override { return m_solutions.size(); }
        const SolutionBase& solutionBase(size_t i) const override { return m_solutions[i]; }

        Solution<TVariable>& solution(size_t i) { return m_solutions[i]; }
        const Solution<TVariable>& solution(size_t i) const { return m_solutions[i]; }
        void appendSolution(const Solution<TVariable> &s) { m_solutions.emplace_back(s); }

        bool isObjectiveGiven() const override {
            return !m_solutions.empty() || OptimaBase::isObjectiveGiven();
        }
        size_t numberObjectives() const override {
            return m_solutions.empty() ? OptimaBase::numberObjectives() : m_solutions.size();
        }
        const std::vector<Real>& objective(size_t i) const override {
            return m_solutions.empty() ? OptimaBase::objective(i) : m_solutions[i].objective();
        }

        void clear() override { OptimaBase::clear(); m_solutions.clear(); }
    };
}
#endif // !OFEC_OPTIMA_H