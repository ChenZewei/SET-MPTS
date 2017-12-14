/*
    Copyright [2016] <Zewei Chen>
*/
#ifndef INCLUDE_RANDOM_GEN_H_
#define INCLUDE_RANDOM_GEN_H_

#include <types.h>
#include <random>
#include <chrono>

class Random_Gen {
 private:
    // static default_random_engine generator;
    static mt19937 generator;

 public:
    // Random_Gen();
    static double exponential_gen(double lambda);
    static int uniform_integral_gen(int min, int max);
    static ulong uniform_ulong_gen(ulong min, ulong max);
    static double uniform_real_gen(double min, double max);
    static bool probability(double prob);
};

#endif  // INCLUDE_RANDOM_GEN_H_
