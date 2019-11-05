/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li & Xia Hai
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*-------------------------------------------------------------------------------
* Test member functions of classes "particle" and "swarm"
*-------------------------------------------------------------------------------
* Created at 2019.09.06 by jcWang
*********************************************************************************/

#include "../utility/catch.hpp"
#include "../instance/algorithm/PSO/SPSO07/SPSO07.h"
#include "../instance/algorithm/PSO/SPSO11/SPSO11.h"
#include "../instance/problem/continuous/global/classical/sphere.h"

using namespace OFEC;

TEST_CASE("Particle Swarm Optimization", "[PSO]") {
    global::ms_global = std::make_unique<global>(0, 0.5, 0.5);
    global::ms_global->m_problem = std::make_unique<sphere>("", 10, 1);

    SECTION("particle") {
        particle p(1, 0 ,10);
        particle p_(p);
        particle p__(particle(1,0,10));
        p_ = p__;
        p_ = particle(1,0,10);
        p.velocity();
        p.initialize_velocity();
        std::vector<std::pair<real, real>> vrange(10,{0,1});
        p.initialize_velocity(vrange);
        p.initialize_velocity(0, 1);
        p.next_velocity(p.solut(), 0, 0, 0);
        p.clamp_velocity();
        p.move();
        p.resize_variable(5);
        p.get_vel();
        p.pbest();
        const particle p___(1, 0 ,10);
        p___.pbest();
    }

    SECTION("particle") {
        swarm<particle> pop(10);
        pop.initialize();
        pop.initialize_pbest();
        pop.evolve();
        pop.weight();
        pop.accelerator1();
        pop.accelerator2();
        pop.average_velocity();
        pop.set_neighborhood();
        pop.neighborhood_best(2);
    }
}

TEST_CASE("Standard PSO 2007", "[SPSO07]") {
    global::ms_global = std::make_unique<global>(0, 0.5, 0.5);
    global::ms_global->m_problem = std::make_unique<sphere>("", 10, 1);

    SECTION("particle") {
        particle07 p(1, 0, 10);
        p.initialize_velocity();
        p.next_velocity(p.solut(), 0,0,0);
    }
}

TEST_CASE("Standard PSO 2011", "[SPSO11]") {
    global::ms_global = std::make_unique<global>(0, 0.5, 0.5);
    global::ms_global->m_problem = std::make_unique<sphere>("", 10, 1);

    SECTION("particle") {
        particle11 p(1, 0, 10);
        p.initialize_velocity();
        p.clamp_velocity();
        p.next_velocity(p.solut(), 0,0,0);
    }
}
