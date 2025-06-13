#pragma once

#include <bnp/core/logger.hpp>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Component {

struct SaveData {
};

struct FakeDataA {
	int value;
};

struct FakeDataB {
	float value;
};

template <typename S>
void serialize(S& s, SaveData& _) {
}

template <typename S>
void serialize(S& s, FakeDataA& a) {
	s.value4b(a.value);
}

template <typename S>
void serialize(S& s, FakeDataB& b) {
	s.value4b(b.value);
}

}
}
}
