#define BOOST_TEST_MODULE first
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(first_test)
{
  int i = 1;
  BOOST_TEST(i);
  BOOST_TEST(i == 2);
}

// int main(int argc, char** args) {


//     return 0;
// }
