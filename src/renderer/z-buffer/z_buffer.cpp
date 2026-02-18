#include "z_buffer.h"
#include <algorithm>
#include <limits>

ZBuffer::ZBuffer(int w, int h) : width(w), height(h) {
	buffer.resize(w * h, std::numeric_limits<float>::max());
}

void ZBuffer::clear() {
	std::ranges::fill(buffer, std::numeric_limits<float>::max());
}

bool ZBuffer::testAndSet(int x, int y, float depth) {
	if (x < 0 || x >= width || y < 0 || y >= height)
		return false;

	int idx = y * width + x;
	if (depth < buffer[idx]) {
		buffer[idx] = depth;
		return true;
	}

	return false;
}

bool ZBuffer::test(int x, int y, float depth) const {
	if (x < 0 || x >= width || y < 0 || y >= height)
		return false;

	int idx = y * width + x;
	return depth < buffer[idx];
}

float ZBuffer::get(int x, int y) const {
	if (x < 0 || x >= width || y < 0 || y >= height)
		return std::numeric_limits<float>::max();

	return buffer[y * width + x];
}




