#include <chrono>
#include <CL/sycl.hpp>
#include <array>
#include <iostream>

using namespace std::chrono;
using namespace cl::sycl;
using namespace sycl;

constexpr access::mode sycl_read = access::mode::read;
constexpr access::mode sycl_write = access::mode::write;
constexpr access::mode sycl_read_write = access::mode::read_write;

template <typename T>
class Totient;
template <typename T>
class TotientReduction;
template <typename T>
class TotientReductionLocal;

// Sequential base

unsigned int hcf(unsigned int x, unsigned int y)
{
  unsigned int t;

  while (y != 0)
  {
    t = x % y;
    x = y;
    y = t;
  }
  return x;
}

// relprime x y = hcf x y == 1

int relprime(unsigned int x, unsigned int y)
{
  return hcf(x, y) == 1;
}

// euler n = length (filter (relprime n) [1 .. n])

unsigned int euler(unsigned int n)
{
  unsigned int length, i;

  length = 0;
  for (i = 1; i <= n; i++)
    if (relprime(n, i))
      length++;
  return length;
}

// sumTotient lower upper = sum (map euler [lower, lower+1 .. upper])

unsigned int sumTotient(unsigned int lower, unsigned int upper)
{
  unsigned int sum, i;

  sum = 0;
  for (i = lower; i <= upper; i++)
    sum = sum + euler(i);
  return sum;
}

int main(int argc, char **argv)
{

  if (argc != 3)
  {
    std::cout << "Not enough arguments : usage ./binary lower upper work_group" << std::endl;
    printf("not 2 arguments\n");
    return 1;
  }

  // lower, upper;
  const size_t lower = atoi(argv[1]);
  const size_t upper = atoi(argv[2]);
  //const size_t wg = atoi(argv[3]);

  // std::cout << "lower " << lower << ", upper " << upper << std::endl;

  //---------------------- Sequential version ----------------------------//

  auto start = high_resolution_clock::now();
  unsigned int sequentialTotient = sumTotient(lower, upper);
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  //std::cout << "sequential: " << sequentialTotient << ", time: " << duration.count() << " seconds" << std::endl;
  std::cout << duration.count() <<std::endl;
  
  // NVidia K20 GPUs have 13 such compute units, each with 192 cores (192 x 13 = 2496 cores).

  //---------------------- SYCL version 1 ----------------------------//

  // const size_t array_size = upper;    // problem sizes
  // std::vector<cl_int> euler_tot(upper);
  // auto start = high_resolution_clock::now();

  // {
  // queue deviceQueue;
  // range<1> numOfItems{array_size};
  // buffer<cl_int, 1> bufferEuler(euler_tot.data(), numOfItems);

  // deviceQueue.submit([&](handler& cgh) {
  //   /* Create an accessor to `bufferEuler` buffer with write access */
  //   auto accessorEuler = bufferEuler.template get_access<sycl_write>(cgh);

  //   auto kern = [=](id<1> wID) {

  //       unsigned int n = wID[0] + 1;
  //       unsigned int length = 0;

  //       // Define hcf function
  //       auto hcf = [](unsigned int x, unsigned int y) -> unsigned int {
  //           while (y != 0) {
  //               unsigned int t = x % y;
  //               x = y;
  //               y = t;
  //           }
  //           return x;
  //       };

  //       // Define relprime function
  //       auto relprime = [&](unsigned int x, unsigned int y) -> int {
  //           return hcf(x, y) == 1;
  //       };

  //       // Euler totient function
  //       for (unsigned int i = 1; i <= n; i++) {
  //           if (relprime(n, i)) {
  //               length++;
  //           }
  //       }

  //       accessorEuler[wID] = length;
  //   };
  //   cgh.parallel_for<class Totient<cl_int>>(numOfItems, kern);
  //   });
  // }

  // unsigned int sycl_1_result = 0;
  // for (unsigned int i = 0; i < array_size; i++) {
  //   sycl_1_result += euler_tot[i];
  // }

  // auto stop = high_resolution_clock::now();
  // auto duration = duration_cast<milliseconds>(stop - start);
  // //std::cout << "SYCL v1: " << sycl_1_result << ", time: " << duration.count() << " ms" << std::endl;
  // std::cout << duration.count() <<std::endl;
  // //---------------------- SYCL version 2 ----------------------------//

  // const size_t array_size = upper; // problem sizes
  // unsigned int sycl_2_result = 0;
  // auto start = high_resolution_clock::now();

  // {
  //   queue deviceQueue;
  //   range<1> numOfItems{array_size};
  //   std::vector<cl_int> euler_tot(upper);
  //   buffer<cl_int, 1> bufferEuler(euler_tot.data(), numOfItems);
  //   buffer<unsigned int> bufferFinalSum{&sycl_2_result, 1};

  //   try
  //   {
  //     deviceQueue.submit([&](handler &cgh){
  //       /* Create an accessor to `bufferEuler` buffer with write access */
  //       auto accessorEuler = bufferEuler.template get_access<sycl_write>(cgh);
  //       auto sumReduction = reduction(bufferFinalSum, cgh, plus<unsigned int>());

  //           auto sum_kern = [=](id<1> wID, auto &sum)
  //       {
  //         unsigned int n = wID[0] + 1;
  //         unsigned int length = 0;

  //         // Define hcf function
  //         auto hcf = [](unsigned int x, unsigned int y) -> unsigned int
  //         {
  //           while (y != 0)
  //           {
  //             unsigned int t = x % y;
  //             x = y;
  //             y = t;
  //           }
  //           return x;
  //         };

  //         // Define relprime function
  //         auto relprime = [&](unsigned int x, unsigned int y) -> int
  //         {
  //           return hcf(x, y) == 1;
  //         };

  //         // Euler totient function
  //         for (unsigned int i = 1; i <= n; i++)
  //         {
  //           if (relprime(n, i))
  //           {
  //             length++;
  //           }
  //         };

  //         sum.combine(length);
  //       };
  //       cgh.parallel_for<class TotientReduction<cl_int>>(numOfItems, sumReduction, sum_kern);});
  //   }
  //   catch (sycl::exception const &e)
  //   {
  //     std::cout << "Caught synchronous SYCL exception:\n"
  //               << e.what() << std::endl;
  //   }
  // }

  // auto stop = high_resolution_clock::now();
  // auto duration = duration_cast<milliseconds>(stop - start);
  // //std::cout << "SYCL v2: " << sycl_2_result << ", time: " << duration.count() << " ms" << std::endl;
  // std::cout << duration.count() <<std::endl;

  // //---------------------- SYCL version 3 ----------------------------//

  // const size_t array_size = upper; // problem sizes
  // unsigned int sycl_3_result = 0;
  // auto start = high_resolution_clock::now();

  // {
  //   queue deviceQueue;

  //   range<1> global_size{array_size};
  //   range<1> wg_size = wg;
  //   nd_range<1> numOfItems{global_size, wg_size};

  //   std::vector<cl_int> euler_tot(upper);
  //   buffer<cl_int, 1> bufferEuler(euler_tot.data(), range<1>(array_size));
  //   buffer<unsigned int> bufferFinalSum{&sycl_3_result, 1};

  //   try
  //   {
  //     deviceQueue.submit([&](handler &cgh){
  //       /* Create an accessor to `bufferEuler` buffer with write access */
  //       auto accessorEuler = bufferEuler.template get_access<sycl_write>(cgh);
  //       auto sumReduction = reduction(bufferFinalSum, cgh, plus<unsigned int>());

  //       auto sum_kern = [=](nd_item<1> item, auto &sum){
  //         auto idx = item.get_global_id(0);
  //         unsigned int n = idx + 1;
  //         unsigned int length = 0;

  //         // Define hcf function
  //         auto hcf = [](unsigned int x, unsigned int y) -> unsigned int
  //         {
  //           while (y != 0)
  //           {
  //             unsigned int t = x % y;
  //             x = y;
  //             y = t;
  //           }
  //           return x;
  //         };

  //         // Define relprime function
  //         auto relprime = [&](unsigned int x, unsigned int y) -> int
  //         {
  //           return hcf(x, y) == 1;
  //         };

  //         // Euler totient function
  //         for (unsigned int i = 1; i <= n; i++)
  //         {
  //           if (relprime(n, i))
  //           {
  //             length++;
  //           }
  //         };

  //         sum.combine(length);
  //       };
  //       cgh.parallel_for<class TotientReductionLocal<cl_int>>(numOfItems, sumReduction, sum_kern);});
  //   }
  //   catch (sycl::exception const &e)
  //   {
  //     std::cout << "Caught synchronous SYCL exception:\n"
  //               << e.what() << std::endl;
  //   }
  // }

  // auto stop = high_resolution_clock::now();
  // auto duration = duration_cast<milliseconds>(stop - start);
  // //std::cout << "SYCL v3: " << sycl_3_result << ", time: " << duration.count() << " ms" << std::endl;
  // std::cout << duration.count() <<std::endl;
}