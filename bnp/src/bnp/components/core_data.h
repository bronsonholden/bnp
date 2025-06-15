#pragma once

namespace bnp {

template <typename ...Components>
struct CoreDataSet {
};

template <typename CoreDataSet>
struct unpack_core_data_set;

template <typename... Components>
struct unpack_core_data_set<CoreDataSet<Components...>> {
	template <typename F>
	static void apply(F&& f) {
		f.template operator() < Components... > ();
	}
};

template <typename... CoreDataSets, typename Fn>
void for_each_core_data_set(Fn&& fn) {
	(unpack_core_data_set<CoreDataSets>::apply(fn), ...);
}

}
