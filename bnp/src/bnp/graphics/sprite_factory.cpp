#include <bnp/graphics/sprite_factory.h>

namespace bnp {

	void SpriteFactory::load_from_aseprite(Node& node, const std::string& json_path) {
		std::ifstream file(json_path);

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open Aseprite JSON file: " + json_path);
		}

		nlohmann::json json;
		file >> json;

		Sprite sprite;

		// Read meta section for sheet size
		const auto& meta = json["meta"];
		sprite.spritesheet_width = meta["size"]["w"];
		sprite.spritesheet_height = meta["size"]["h"];

		// Determine frame size from first frame entry
		const auto& frames = json["frames"];
		if (frames.empty()) throw std::runtime_error("No frames in Aseprite JSON.");

		// Map frame index to SpriteFrame
		std::vector<std::string> frame_keys;
		for (const auto& item : frames.items()) frame_keys.push_back(item.key());
		std::sort(frame_keys.begin(), frame_keys.end());

		// Parse animations from frameTags
		if (meta.contains("frameTags")) {
			for (const auto& tag : meta["frameTags"]) {
				std::string name = tag["name"];
				int from = tag["from"];
				int to = tag["to"];

				SpriteAnimation anim;
				anim.name = name;
				anim.frames.reserve(to - from + 1);

				for (int i = from; i <= to; ++i) {
					const auto& frame_data = frames.at(frame_keys[i]);
					auto duration = frame_data["duration"].get<float>() / 1000.0f;

					auto x = frame_data["frame"]["x"].get<int>();
					auto y = frame_data["frame"]["y"].get<int>();
					auto w = frame_data["frame"]["w"].get<int>();
					auto h = frame_data["frame"]["h"].get<int>();

					SpriteFrame frame;
					frame.frame_index = i;
					frame.duration = duration;
					frame.size = { w, h };
					frame.uv0 = glm::vec2((float)x / sprite.spritesheet_width, (float)(y) / sprite.spritesheet_height);
					frame.uv1 = glm::vec2((float)(x + w) / sprite.spritesheet_width, (float)(y + h) / sprite.spritesheet_height);

					anim.frames.push_back(frame);
				}

				sprite.animations[name] = anim;
			}
		}

		// Store frame size from the first parsed frame
		if (!sprite.animations.empty()) {
			glm::ivec2 size = sprite.animations.begin()->second.frames.front().size;
			sprite.frame_width = size.x;
			sprite.frame_height = size.y;
		}
		else {
			sprite.frame_width = sprite.spritesheet_width;
			sprite.frame_height = sprite.spritesheet_width;
		}

		node.add_component<Sprite>(sprite);

		SpriteAnimator animator;
		if (!sprite.animations.empty()) {
			animator.current_animation = sprite.animations.begin()->first;
			animator.current_frame_index = 0;
			animator.playing = true;
			node.add_component<SpriteAnimator>(animator);
		}
	}

}
