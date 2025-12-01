#ifndef ZBUFFER_H
#define ZBUFFER_H

#include <vector>
#include <mutex>
#include <memory>

class ZBuffer {
public:
  ZBuffer(int w, int h);

  void clear();

  bool testAndSet(int x, int y, float depth);

  bool test(int x, int y, float depth) const;

  float get(int x, int y) const;
private:
  std::vector<float> buffer;
  std::vector<std::unique_ptr<std::mutex>> rowMutexes;
  int width;
  int height;
};

#endif // ZBUFFER_H