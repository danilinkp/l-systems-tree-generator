#ifndef L_SYSTEM_GENERATOR_H
#define L_SYSTEM_GENERATOR_H

#include <QDebug>

class LSystemGenerator {
public:
	LSystemGenerator();

	void setAxiom(const QString &ax);
	void addRule(QChar symbol, const QString &replacement);
	void setIterations(int n);

	QString generate() const;
private:
	QString axiom;
	QMap<QChar, QString> rules;
	int iterations;
};

#endif // L_SYSTEM_GENERATOR_H