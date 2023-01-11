#include "global.hpp"
#include "input.hpp"
#include "simulation.hpp"
#include "output.hpp"
#include "estimation.hpp"


int main()
{
    std::vector<double> b1 = {5.75102, 1.3194, 5.20099, 1.4603};
    std::ofstream ofs(DIR + "revenue.csv");
    // std::cout << ExpectedRevenue(10000, b1) << std::endl;
    // b1 = ApproxValueIteration();
    // ValueIteration();

    ofs << "Points,Revenue" << std::endl;
    for (int s = 0; s <= MAX_POINT; s += 1000) {
        ofs << s << "," << ExpectedRevenue(s, b1) << std::endl;
    }

    std::cout << "done!" << std::endl;

    return 0;

    double sumCost;
    std::vector<std::vector<double>> c;
    std::vector<bool> overTrans;
    std::vector<double> sumCostTrans, b(BASE_NUM);
    std::vector<std::vector<double>> pTrans;
    std::vector<std::vector<std::vector<double>>> xTrans, cTrans;
    std::function<double(int)> DelayCostObject = DelayCost;
    std::function<double(double, double)> LinkCostFuncObject = LinkCostFunc, LinkCostFuncPrimeObject = LinkCostFuncPrime;

    // 施設データ読み込み・セット（ノード数カウント）
    std::vector<std::vector<SpatialLink>> Link = ReadNetworkFile(DIR + SUBDIR + NW_FNAME);
    std::cout << "施設データ読み込み完了" << std::endl;

    // 集団読み込み・セット
    std::vector<Population> P = ReadPopulationFile(DIR + SUBDIR + POP_FNAME);
    std::cout << "集団データ読み込み完了" << std::endl;


    // ポイント保有価値関数推定
    b = ApproxValueIteration();


    // 需要制御期シミュレーション
    std::vector<std::vector<double>> init_x;
    init_x = makeInitDist(P);
    OptimizeWithBarrier(Link, init_x, P, DelayCost, LinkCostFunc, LinkCostFuncPrime, sumCostTrans, xTrans, overTrans, cTrans, pTrans, b);


    // ポイント使用期シミュレーション


    // CSV出力
    WriteSumCostFile(sumCostTrans, DIR + SUBDIR + SUMCOST_FNAME);
    WriteXCTransFile(xTrans, cTrans, DIR + SUBDIR + X_FNAME, DIR + SUBDIR + C_FNAME);
    WriteOverTransFile(overTrans, DIR + SUBDIR + OVER_FNAME);
    WriteParameterFile();

    return 0;
}