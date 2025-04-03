#include <random>

namespace bnp {

	class RandomFloatGenerator {
	public:
		// Constructor to set the range [min, max]
		RandomFloatGenerator(float min, float max)
			: engine(std::random_device{}()), distribution(min, max) {}

		// Generate a random float within the range
		float generate() {
			return distribution(engine);
		}

	private:
		std::mt19937 engine;
		std::uniform_real_distribution<float> distribution;
	};

} // namespace bnp
