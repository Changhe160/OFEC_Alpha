/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
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
* class factory is for the registration of all problems and algorithms  
*
*********************************************************************************/

#ifndef OFEC_FACTORY_H
#define OFEC_FACTORY_H

#include <map>
#include <functional>
#include "../core/definition.h"

namespace OFEC {

	template<typename Base>
	struct factory
	{
		template<typename T>
		struct register_
		{
			register_(const std::string& key, std::set<problem_tag>&& tag)
			{
				map_.emplace(key, make_pair([](param_map& par) { return new T(par); }, std::forward<std::set<problem_tag>>(tag)));
			}
		};

		static Base* produce(const std::string& key, typename param_map& par)
		{
			if (map_.find(key) == map_.end())
				THROW("the key is not exist!");

			return map_[key].first(par);
		}

		static const std::map<std::string, std::pair<std::function<Base*(param_map&)>, std::set<problem_tag>> >& get()
		{		
			return map_;
		}
		factory() =default;
	private:
		factory& operator=(const factory&) = delete;
		factory& operator=(factory&&) = delete;
		factory(const factory&) = delete;
		factory(factory&&) = delete;
		static std::map<std::string, std::pair<std::function<Base*(param_map&)>,std::vector<problem_tag>> > map_;
	};

	template<typename Base>
	std::map<std::string, std::pair<std::function<Base*(param_map&)>, std::vector<problem_tag>> > factory<Base>::map_;

	#define RIGIESTER(Base, Derived, key, tag) factory<Base>::register_<Derived> reg_##Derived(key, tag);

}

#endif
