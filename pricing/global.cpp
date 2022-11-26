#include "global.hpp"

const int MAX_ITER = 1000000;  // 繰り返し計算の上限
const double CONVERGE_THRES = 0.00001;  // 収束判定用
const int CONVERGE_COUNT_THRES = 300000;  // 収束判定用
const double MAX_NORM = 500;  // 勾配の最大値
const double CAP_COMP = 2.5;  // BPR関数の容量は定員の何分の1か
const double SPAN = 5;  // 列車の運行間隔
const double NU1 = 0.001; // 総コスト最小化ステップのバリアパラメータ
const double NU2 = 0.05;  // 待ち行列除去ステップのパラメータ
const int T_BAR = 2;  // 時間帯上限
const int T_STAR = 1;  // 希望到着時刻
const double A = 0.5;
const double B = 1.2;  // BPR関数の係数
const double REMAIN_COST = 100;  // 移動失敗者のコスト
const double BETA_W = 0.15;  // 待ち時間の時間価値
const double BETA_H = 0.08;  // 乗車時間の時間価値
const double BETA_E = 0.12;  // 早着に関する時間価値
const double BETA_L = 1;  // 遅着に関する時間価値
const double DELTA = 1e-6;  // 微小時間
const double CAP = 100;  // 列車の定員
const std::string DIR = "/home/suzuki/master/221126/";
const std::string SUBDIR = "1/";
const std::string NW_FNAME = "network.csv";
const std::string POP_FNAME = "population.csv";
const std::string SUMCOST_FNAME = "sumCostTrans.csv";
const std::string X_FNAME = "xTrans.csv";
const std::string OVER_FNAME = "overTrans.csv";
const std::string C_FNAME = "cTrans.csv";
const std::string PARAM_FNAME = "parameters.csv";


void SpatialLink::CalcUser(std::vector<std::vector<double>> x) {
    int i, j;

    this->u = 0;
    for (i = 0; i <= this->spaceIdx; i++) {
        this->u += x[this->timeIdx][i];
    }
}


void SpatialLink::CalcCost(std::function<double(double, double)> LinkCostFunc) {
    this->cost = LinkCostFunc(this->u, this->t);
}


void SpatialLink::CalcCostPrime(std::function<double(double, double)> LinkCostFuncPrime) {
    this->cost_prime = LinkCostFuncPrime(this->u, this->t);
}


void SpatialLink::CalcLinkPrice() {
    this->price = this->cost_prime * this->u;
}


// スケジューリングコスト
double DelayCost(int t)
{
    if (t < T_STAR) {
        return BETA_E * (T_STAR - t);
    } else {
        return BETA_L * (t - T_STAR);
    }
}


// リンクコスト関数
double LinkCostFunc(double u, double t)
{
    return BETA_H * (1 + A * std::pow(u * CAP_COMP / CAP, B)) * t;
}


// リンクコスト関数の微分
double LinkCostFuncPrime(double u, double t)
{
    return BETA_H * A * B * std::pow(u * 2.5 / CAP, B - 1) * (CAP_COMP / CAP) * t;
}