#include "global.hpp"

const int MAX_ITER = 1000000;  // 繰り返し計算の上限
const double CONVERGE_THRES = 0.00001;  // 収束判定用
const int CONVERGE_COUNT_THRES = 300000;  // 収束判定用
const double MAX_NORM = 100;  // 勾配の最大値
const double CAP_COMP = 2.5;  // BPR関数の容量は定員の何分の1か
const double SPAN = 5;  // 列車の運行間隔
const double NU1 = 0.01; // 総コスト最小化ステップのバリアパラメータ
const double NU2 = 10000;  // 待ち行列除去ステップのパラメータ
const double KAPPA = 0.01;
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
const double CAP = 83;  // 列車の定員
const int ACTION_NUM = 50;
const std::vector<double> U_COM = []() {
    std::vector<double> v(ACTION_NUM);
    for (int i = 0; i < ACTION_NUM; i++) {
        v[i] = 10000.0 * std::sqrt(i);
    }
    return v;
}();  // 商品から得られる効用
const std::vector<double> P = []() {
    std::vector<double> v(ACTION_NUM);
    for (int i = 0; i < ACTION_NUM; i++) {
        v[i] = 1000.0 * i;
    }
    return v;
}();  // 価格
const double BETA_T = 0.2;  // 価値関数の時間割引率. 0.5
const double R = 0.01;  // 店Aのポイント還元率.0.01
const int K_THRES = 1000000;  // 価値反復1回あたりのサンプリング回数
const double MAX_POINT = 50000;  // サンプリング時の上限値（下限値は0で良い）
const double VALUE_THRES = 1e-4;  // 価値反復の収束判定閾値.1e-4
const double ALPHA = 0.00000001;  // 学習率. 0.00000001
const double GAMMA = 0.7;  // ポイントの相対的価値.0.7
const int BOOTSTRAP_N = 10;
const int BASE_NUM = 4;
const double MAX_SCALE = 300;
const double B0_SCALE = 10000;  // 10000
const double B1_SCALE = 1000;  // 10000
const double B2_SCALE = 0.00001;  // 0.000001
const double B3_SCALE = B2_SCALE * 100000;
const int MONTE_NUM = 2000000;
const int MONTE_DUR = 5;
const double CHEAP = 0.7;
const double MU = 3000;
const std::vector<std::function<double(double)>> BASE_FUNC = {
    [](double x) -> double {
        return 1.0;
    },
    // [](double x) -> double {
    //     return std::tanh(0.001 * x);
    // },
    //     [](double x) -> double {
    //     return std::tanh(0.002 * x);
    // },
    //     [](double x) -> double {
    //     return std::tanh(0.003 * x);
    // },
    //     [](double x) -> double {
    //     return std::tanh(0.004 * x);
    // },
    //     [](double x) -> double {
    //     return std::tanh(0.005 * x);
    // },
    //     [](double x) -> double {
    //     return std::tanh(0.006 * x);
    // },
    //     [](double x) -> double {
    //     return std::tanh(0.007 * x);
    // },
    //     [](double x) -> double {
    //     return std::tanh(0.008 * x);
    // },
    //     [](double x) -> double {
    //     return std::tanh(0.009 * x);
    // },
        [](double x) -> double {
        return std::tanh(0.01 * x);
    },
};
const std::string DIR = "/home/suzuki/master/point/";
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
    this->price = KAPPA * this->cost_prime;
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


// プロスペクト理論の価値関数
double ProspectValue(double x)
{
    if (x >= 0) {
        return x;
        // return std::sqrt(std::fabs(x));
    } else {
        return x;
        // return -1.5 * std::sqrt(std::fabs(x));
    }
}