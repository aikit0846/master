#include "global.hpp"
#include "input.hpp"
#include "simulation.hpp"
#include "output.hpp"


int main()
{
    double sumCost;
    std::vector<std::vector<double>> c;
    std::vector<bool> overTrans;
    std::vector<double> sumCostTrans;
    std::vector<std::vector<std::vector<double>>> xTrans, cTrans;
    std::function<double(int)> DelayCostObject = DelayCost;
    std::function<double(double, double)> LinkCostFuncObject = LinkCostFunc, LinkCostFuncPrimeObject = LinkCostFuncPrime;

    // 施設データ読み込み・セット（ノード数カウント）
    std::vector<std::vector<SpatialLink>> Link = ReadNetworkFile(DIR + SUBDIR + NW_FNAME);
    std::cout << "施設データ読み込み完了" << std::endl;

    // 集団読み込み・セット
    std::vector<Population> P = ReadPopulationFile(DIR + SUBDIR + POP_FNAME);
    std::cout << "集団データ読み込み完了" << std::endl;

    //// シミュレーション
    std::vector<std::vector<double>> init_x;
    init_x = makeInitDist(P);
    OptimizeWithBarrier(Link, init_x, P, DelayCost, LinkCostFunc, LinkCostFuncPrime, sumCostTrans, xTrans, overTrans, cTrans);


    // CSV出力
    WriteSumCostFile(sumCostTrans, DIR + SUBDIR + SUMCOST_FNAME);
    WriteXCTransFile(xTrans, cTrans, DIR + SUBDIR + X_FNAME, DIR + SUBDIR + C_FNAME);
    WriteOverTransFile(overTrans, DIR + SUBDIR + OVER_FNAME);
    WriteParameterFile();

    return 0;
}