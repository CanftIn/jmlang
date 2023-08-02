#include <iostream>

#include "jmlang/jmlang.h"

int main() {
  jmlang::Func gradient;
  jmlang::Var x, y;
  jmlang::Expr e = x + y;
  gradient(x, y) = e;
  jmlang::Image<int32_t> output = gradient.realize(800, 600);

  for (int i = 0; i < output.height(); i++) {
    for (int j = 0; j < output.width(); j++) {
      std::cout << "Pixel(" << i << ", " << j << "): " << i + j
                << ", output: " << output(i, j) << "\n";
    }
  }
  return 0;
}