#pragma once

namespace Utility{

	struct Point {
		int x, y;
		Point() { x = y = 0; }
	};

	struct Rect {
		int x, y, w, h;
		Rect() { x = y = w = h = 0; }
	};

	struct Circle {
		Point center;
		int r;
		Circle() { r = 0; }
	};

}