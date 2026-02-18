void Rasterizer::drawScanline(int y, const ScreenVertex &left, const ScreenVertex &right, const glm::vec3 &cameraPos) {
	if (left.position.x > right.position.x)
		return;
	float dx = right.position.x - left.position.x;

	if (dx < 0.1f) {
		int x = (int)(left.position.x + 0.5f);
		if (x >= 0 && x < image.width())
			if (zBuffer->testAndSet(x, y, left.depth)) {
				QColor color = calculateColor(left, cameraPos);
				image.setPixel(x, y, color.rgb());
				pixelsDrawn++;
			}
		return;
	}
	int xStart = std::max(0, static_cast<int>(std::floor(left.position.x + 0.5f)));
	int xEnd = std::min(image.width() - 1, static_cast<int>(std::floor(right.position.x + 0.5f)));
	float invDx = 1.0f / dx;
	QRgb* scanline = reinterpret_cast<QRgb*>(image.scanLine(y));

	for (int x = xStart; x <= xEnd; x++) {
		float px = x + 0.5f;
		float t = (px - left.position.x) * invDx;
		t = glm::clamp(t, 0.0f, 1.0f);
		ScreenVertex pixel = interpolate(left, right, t);
		if (pixel.depth >= 0.0f && pixel.depth <= 1.0f) {
			if (pixel.depth < 0.01f)
				continue;
			if (zBuffer->testAndSet(x, y, pixel.depth)) {
				QColor color = calculateColor(pixel, cameraPos);
				scanline[x] = color.rgb();
				pixelsDrawn++;
			}
		}
	}
}