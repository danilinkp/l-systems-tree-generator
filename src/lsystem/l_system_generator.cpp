#include "l_system_generator.h"

LSystemGenerator::LSystemGenerator()
	: axiom("F"), iterations(3) {
	rules['F'] = "F[+F]F[-F]F";
}

void LSystemGenerator::setAxiom(const QString &ax) {
	axiom = ax;
}

void LSystemGenerator::addRule(QChar symbol, const QString &replacement) {
	rules[symbol] = replacement;
}

void LSystemGenerator::setIterations(int n) {
	iterations = n;
}

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