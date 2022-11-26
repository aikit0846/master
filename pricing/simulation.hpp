#ifndef SIMULATION_HPP_
#define SIMULATION_HPP_

#include "global.hpp"

// 各戦略の含めたコストを計算
std::vector<std::vector<double>> CalcCostWithQueue(std::vector<std::vector<SpatialLink>> &Link, std::vector<std::vector<double>> x, std::function<double(int)> DelayCost, std::function<double(double, double)> LinkCostFunc, std::function<double(double, double)> LinkCostFuncPrime);

// The Proportional-Switch Adjustment Process
// 現在の状態と利得関数を受け取って，微小時間後の状態を返す関数
std::vector<std::vector<double>> PSAP(std::vector<std::vector<double>> x, std::vector<Population> P, std::vector<std::vector<double>> c);

// 勾配クリッピング
std::vector<std::vector<double>> GradientClipping(std::vector<std::vector<double>> c);

// 最適化
std::vector<std::vector<double>> OptimizeWithBarrier(std::vector<std::vector<SpatialLink>> Link, std::vector<std::vector<double>> init_x, std::vector<Population> P, std::function<double(int)> DelayCost, std::function<double(double, double)> LinkCostFunc, std::function<double(double, double)> LinkCostFuncPrime, std::vector<double> &sumCostTrans, std::vector<std::vector<std::vector<double>>> &xTrans, std::vector<bool> &overTrans, std::vector<std::vector<std::vector<double>>> &cTrans);

// 課金なしのシミュレーション
void SimulationWithout(std::vector<std::vector<SpatialLink>> Link, std::vector<std::vector<double>> init_x, std::vector<Population> P, std::function<double(int)> DelayCost, std::function<double(double, double)> LinkCostFunc, std::function<double(double, double)> LinkCostFuncPrime, std::vector<double> &sumCostTrans, std::vector<std::vector<std::vector<double>>> &xTrans);

// 初期解生成
std::vector<std::vector<double>> makeInitDist(std::vector<Population> P);

#endif