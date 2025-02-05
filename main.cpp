#include <iostream>

#include "create.h"




static_assert(std::is_same_v<decltype(CREATE(a = 2, b = 1)), decltype(CREATE(a = 52, b = 1))>);

int main() {
  auto x = CREATE(a = 52, b = 142);
  std::cout << x.b << std::endl;
}


