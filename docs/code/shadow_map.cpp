void ShadowMapRenderer::fillTriangle(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, float z0, float z1, float z2) {
	int minX = std::max(0, (int)std::floor(std::min({v0.x, v1.x, v2.x})));
	int maxX = std::min(width - 1, (int)std::ceil(std::max({v0.x, v1.x, v2.x})));
	int minY = std::max(0, (int)std::floor(std::min({v0.y, v1.y, v2.y})));
	int maxY = std::min(height - 1, (int)std::ceil(std::max({v0.y, v1.y, v2.y})));

	for (int y = minY; y <= maxY; ++y) {
		for (int x = minX; x <= maxX; ++x) {
			glm::vec2 p(x + 0.5f, y + 0.5f);

			float det = (v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y);
			if (std::abs(det) < 1e-8f) continue;

			float w0 = ((v1.y - v2.y) * (p.x - v2.x) + (v2.x - v1.x) * (p.y - v2.y)) / det;
			float w1 = ((v2.y - v0.y) * (p.x - v2.x) + (v0.x - v2.x) * (p.y - v2.y)) / det;
			float w2 = 1.0f - w0 - w1;

			if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
				float z = w0 * z0 + w1 * z1 + w2 * z2;
				z = glm::clamp(z, 0.0f, 1.0f);

				int idx = y * width + x;
				if (idx >= 0 && idx < static_cast<int>(shadowMap.size()))
					if (zBuffer->testAndSet(x, y, z))
						shadowMap[idx] = z;
			}
		}
	}
}