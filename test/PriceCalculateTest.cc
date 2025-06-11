#include "ltps/common/PriceCalculate.h"
#include <iostream>
#include <string>

namespace tps::test {


void PriceCalculateTest() {
    PriceCalculate cl{"random_num() * n"};
    cl.addVariable("n", 2);
    auto val = cl.eval();

    std::cout << "val: " << (val.has_value() ? std::to_string(*val) : "null") << std::endl;

    PriceCalculate cl2{"random_num()"};
    auto           val2 = cl2.eval();
    std::cout << "val2: " << (val2.has_value() ? std::to_string(*val2) : "null") << std::endl;

    PriceCalculate cl3{"random_num_range(1, 10)"};
    auto           val3 = cl3.eval();
    std::cout << "val3: " << (val3.has_value() ? std::to_string(*val3) : "null") << std::endl;
}


} // namespace tps::test
