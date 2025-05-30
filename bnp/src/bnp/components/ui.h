#pragma once

namespace bnp {

struct Button {
	bool enabled = true;
	bool clicked = false;
};

struct Hoverable {
	bool hovered = false;
};

}
