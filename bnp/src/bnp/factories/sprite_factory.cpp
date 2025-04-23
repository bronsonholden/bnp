#include <bnp/factories/sprite_factory.h>
#include <bnp/components/physics.h>

namespace bnp {

	// todo: maybe this should fn go to resource manager
	void SpriteFactory::load_from_aseprite(Node& node, const std::filesystem::path& json_path) {
		std::filesystem::path root = std::filesystem::path(PROJECT_ROOT) / "bnp";
		std::filesystem::path path = root / json_path;
		std::ifstream file(path);

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open Aseprite JSON file: " + path.string());
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

		for (int i = 0; i < frames.size(); ++i) {
			const auto& frame_data = frames.at(i);
			auto duration = frame_data["duration"].get<float>() / 1000.0f;

			auto x = frame_data["frame"]["x"].get<int>();
			auto y = frame_data["frame"]["y"].get<int>();
			auto w = frame_data["frame"]["w"].get<int>();
			auto h = frame_data["frame"]["h"].get<int>();

			SpriteFrame frame;
			frame.frame_index = i;
			frame.duration = duration;
			frame.size = { w, h };
			frame.uv0 = glm::vec2((float)x / sprite.spritesheet_width, (float)(y + h) / sprite.spritesheet_height);
			frame.uv1 = glm::vec2((float)(x + w) / sprite.spritesheet_width, (float)(y) / sprite.spritesheet_height);
			frame.coords = glm::ivec4(x, y, w, h);

			sprite.frames.push_back(frame);
		}

		// preload collider slices
		std::unordered_map<int, glm::ivec4> collider_by_frame;

		// if no body slice defined
		glm::ivec4 body = load_body_slice(meta);

		// Parse animations from frameTags
		if (meta.contains("frameTags")) {
			for (const auto& tag : meta["frameTags"]) {
				std::string name = tag["name"];
				int from = tag["from"];
				int to = tag["to"];

				SpriteAnimation anim;
				anim.name = name;
				anim.framelist.reserve(to - from + 1);

				// todo: support actual repeat count
				if (tag.contains("repeat")) {
					anim.repeat = 1;
				}

				for (int i = from; i <= to; ++i) {
					const auto& frame_data = frames.at(i);
					auto duration = frame_data["duration"].get<float>() / 1000.0f;

					auto x = frame_data["frame"]["x"].get<int>();
					auto y = frame_data["frame"]["y"].get<int>();
					auto w = frame_data["frame"]["w"].get<int>();
					auto h = frame_data["frame"]["h"].get<int>();

					SpriteFrame frame;
					frame.frame_index = i;
					frame.duration = duration;
					frame.size = { w, h };
					frame.uv0 = glm::vec2((float)x / sprite.spritesheet_width, (float)(y + h) / sprite.spritesheet_height);
					frame.uv1 = glm::vec2((float)(x + w) / sprite.spritesheet_width, (float)(y) / sprite.spritesheet_height);
					frame.coords = glm::ivec4(x, y, w, h);

					anim.framelist.push_back(i);
				}

				sprite.animations[name] = anim;
			}
		}

		if (meta.contains("slices")) {
			for (const auto& slice : meta["slices"]) {
				std::string name = slice["name"];

				std::unordered_map<uint32_t, glm::ivec4> slice_keys;

				for (const auto& key : slice["keys"]) {
					auto x = key["bounds"]["x"].get<int>();
					auto y = key["bounds"]["y"].get<int>();
					auto w = key["bounds"]["w"].get<int>();
					auto h = key["bounds"]["h"].get<int>();
					slice_keys.emplace(key["frame"].get<uint32_t>(), glm::ivec4(x, y, w, h));
				}

				sprite.slices.emplace(name, slice_keys);
			}
		}

		uint32_t i = 0;
		for (const auto& layer : meta["layers"]) {
			std::string name = layer["name"];

			sprite.layers.push_back(SpriteLayer{
				name,
				i,
				i == 0
				});

			++i;
		}

		sprite.frame_count = frames.size() / meta["layers"].size();

		// Store frame size from the first parsed frame. todo: probably fix, but might
		// use consistent frame sizes for all spritesheets anyways
		if (!sprite.animations.empty()) {
			uint32_t frame_index = sprite.animations.begin()->second.framelist.front();
			sprite.default_frame = sprite.frames.at(frame_index);
			glm::ivec2 size = sprite.default_frame.size;
			sprite.frame_width = size.x;
			sprite.frame_height = size.y;
		}
		else {
			sprite.frame_width = sprite.spritesheet_width;
			sprite.frame_height = sprite.spritesheet_width;
			sprite.default_frame = SpriteFrame{
				0,
				0.0f,
				{ 0.0f, 1.0f },
				{ 1.0f, 0.0f },
				glm::ivec2{},
				glm::ivec2{ sprite.spritesheet_width, sprite.spritesheet_height }
			};
		}

		node.add_component<Sprite>(sprite);

		SpriteAnimator animator;
		if (!sprite.animations.empty()) {
			animator.current_animation = sprite.animations.begin()->first;
			animator.current_framelist_index = 0;
			animator.playing = true;
			node.add_component<SpriteAnimator>(animator);
		}
	}

	glm::ivec4 SpriteFactory::load_body_slice(const nlohmann::json& meta) {
		int spritesheet_width = meta["size"]["w"];
		int spritesheet_height = meta["size"]["h"];

		glm::ivec4 default_body(0);

		if (!meta.contains("slices")) {
			return default_body;
		}

		for (const auto& slice : meta["slices"]) {
			if (slice.contains("name") && slice["name"] == "Body" && slice.contains("keys")) {
				for (const auto& key : slice["keys"]) {
					if (!key.contains("frame") || !key.contains("bounds") || key["frame"] != 0) {
						return default_body;
					}

					int frame_index = key["frame"];
					const auto& bounds = key["bounds"];

					return glm::ivec4(
						bounds["x"].get<int>(),
						bounds["y"].get<int>(),
						bounds["w"].get<int>(),
						bounds["h"].get<int>()
					);

				}
			}
		}

		return default_body;
	}

}
