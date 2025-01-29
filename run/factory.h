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
#include <set>
#include <string>
#include <functional>
#include "../core/exception.h"

namespace ofec {
	template<typename TBase>
	struct Factory {
		static std::set<std::string> ms_tags;

		template<typename TDerived>
		struct register_ {
			register_(const std::string& key, std::set<std::string>&& tags) {
				ms_tags.insert(tags.begin(), tags.end());
				map_.emplace(
					key,
					make_pair([]() {
						return TDerived::create();
						},
						std::forward<std::set<std::string>>(tags))
				);
			}
		};

		static TBase* produce(const std::string& key) {
			auto it = map_.find(key);
			if (it == map_.end()) {
				throw Exception("the key \"" + key + "\" is not exist!");
			}
			auto ptr = it->second.first();
			ptr->setName(key);
			return ptr;
		}

		static const std::map<std::string, std::pair<std::function<TBase* ()>, std::set<std::string>>>& get() {
			return map_;
		}

		Factory() = default;
	private:
		Factory& operator=(const Factory&) = delete;
		Factory& operator=(Factory&&) = delete;
		Factory(const Factory&) = delete;
		Factory(Factory&&) = delete;
		static std::map<std::string, std::pair<std::function<TBase* ()>, std::set<std::string>>> map_;
	};

	template<typename TBase>
	std::map<std::string, std::pair<std::function<TBase* ()>, std::set<std::string>>> Factory<TBase>::map_;
	template<typename TBase>
	std::set<std::string> Factory<TBase>::ms_tags;

#define REGISTER(TBase, TDerived, key, tags) Factory<TBase>::register_<TDerived> reg_##TDerived(key, tags)
}
#endif
