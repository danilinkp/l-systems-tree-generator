#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QDir>

#include "benchmark_runner.h"

int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);
	BenchmarkRunner runner;
	runner.runFullSuite("../data/benchmark_result.csv");

	return 0;
}