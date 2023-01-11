#ifndef SIMULATION_HPP_
#define SIMULATION_HPP_

#include "global.hpp"

// The Proportional-Switch Adjustment Process
double FlowDynamics(std::vector<Path> &pathVec, std::vector<Demand> demandVec, std::vector<std::vector<double>> &perturbationTrans);

// 保有ポイント数のダイナミクス
void PointDynamics(std::vector<Path> pathVec, std::vector<Link> linkVec, std::vector<Demand> &demandVec);

// ランダムな初期解生成
void MakeRandomInitFlow(std::vector<Path> &pathVec, std::vector<Demand> demandVec);

// 初期解生成
void MakeInitFlow(std::vector<Path> &pathVec, std::vector<Link> &linkVec, std::vector<Demand> demandVec, std::function<double(double, double)> RailLinkCostFunc, std::function<double(double, double)> StayLinkCostFunc);

// 金銭での課金額とポイント付与額決定
void CalcMoneyPoint(std::vector<Path> pathVec, std::vector<Link> &linkVec, std::vector<Node> nodeVec, std::vector<Demand> demandVec);

// With Pointでシミュレーション
void Simulation(std::vector<Path> &pathVec, std::vector<Link> &linkVec, std::vector<Node> nodeVec, std::vector<Demand> &demandVec, std::function<double(double, double)> RailLinkCostFunc, std::function<double(double, double)> RailLinkCostFuncPrime, std::function<double(double, double)> StayLinkCostFunc, std::vector<double> &sumCostTrans, std::vector<double> &railCostTrans, std::vector<double> &stayUtilTrans, std::vector<std::vector<double>> &pathCostTrans, std::vector<std::vector<double>> &possessedPointTrans, std::vector<std::vector<double>> &linkFlowTrans, std::vector<std::vector<double>> &zeroFlowTrans, std::vector<std::vector<double>> &oneFlowTrans, std::vector<std::vector<double>> &pathFlow, std::vector<std::vector<double>> &moneyTrans, std::vector<std::vector<double>> &pointTrans, std::vector<double> &railRevenueTrans, std::vector<double> &storeRevenueTrans, std::vector<std::vector<double>> &perturbationTrans, std::vector<std::vector<double>> &pathZeroCostTrans, std::vector<std::vector<double>> &pathOneCostTrans);

// Without Pointでシミュレーション
void SimulationWithout(std::vector<Path> &pathVec, std::vector<Link> &linkVec, std::vector<Demand> &demandVec, std::function<double(double, double)> RailLinkCostFunc, std::function<double(double, double)> RailLinkCostFuncPrime, std::function<double(double, double)> StayLinkCostFunc, std::vector<double> &sumCostTrans, std::vector<double> &railCostTrans, std::vector<double> &stayUtilTrans, std::vector<std::vector<double>> &pathCostTrans, std::vector<std::vector<double>> &possessedPointTrans, std::vector<std::vector<double>> &linkFlowTrans, std::vector<std::vector<double>> &zeroFlowTrans, std::vector<std::vector<double>> &oneFlowTrans, std::vector<std::vector<double>> &pathFLowTrans, std::vector<std::vector<double>> &moneyTrans, std::vector<std::vector<double>> &pointTrans, std::vector<double> &railRevenueTrans, std::vector<double> &storeRevenueTrans, std::vector<std::vector<double>> &perturbationTrans, std::vector<std::vector<double>> &pathZeroCostTrans, std::vector<std::vector<double>> &pathOneCostTrans);

// Without Controlのシミュレーション
void SimulationNoControl(std::vector<Path> &pathVec, std::vector<Link> &linkVec, std::vector<Node> nodeVec, std::vector<Demand> &demandVec, std::function<double(double, double)> RailLinkCostFunc, std::function<double(double, double)> RailLinkCostFuncPrime, std::function<double(double, double)> StayLinkCostFunc, std::vector<double> &sumCostTrans, std::vector<double> &railCostTrans, std::vector<double> &stayUtilTrans, std::vector<std::vector<double>> &pathCostTrans, std::vector<std::vector<double>> &possessedPointTrans, std::vector<std::vector<double>> &linkFlowTrans, std::vector<std::vector<double>> &zeroFlowTrans, std::vector<std::vector<double>> &oneFlowTrans, std::vector<std::vector<double>> &pathFlowTrans, std::vector<std::vector<double>> &moneyTrans, std::vector<std::vector<double>> &pointTrans, std::vector<double> &railRevenueTrans, std::vector<double> &storeRevenueTrans, std::vector<std::vector<double>> &perturbationTrans, std::vector<std::vector<double>> &pathZeroCostTrans, std::vector<std::vector<double>> &pathOneCostTrans);

#endif