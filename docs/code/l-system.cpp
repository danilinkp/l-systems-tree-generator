QString LSystemGenerator::generate() const {
	QString current = axiom;

	for (int i = 0; i < iterations; i++) {
		QString next;

		for (const QChar &c : current) {
			if (rules.contains(c))
				next += rules[c];
			else
				next += c;
		}
		current = next;
	}

	return current;
}