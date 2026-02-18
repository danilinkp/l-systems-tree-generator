#ifndef ZBUFFER_H
#define ZBUFFER_H

#include <vector>

class ZBuffer {
public:
  ZBuffer(int w, int h);

  void clear();

  bool testAndSet(int x, int y, float depth);

  bool test(int x, int y, float depth) const;

  float get(int x, int y) const;

private:
  std::vector<float> buffer;
  int width;
  int height;
};

#endif // ZBUFFER_H