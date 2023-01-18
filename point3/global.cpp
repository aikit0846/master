#include "global.hpp"

// const int MODE = 0;
const bool PATH_MODE = false;  // パスベースの変数を記録するか否か（メモリ圧迫防止）
const int SIM_NUM = 100; // 3000;  // シミュレーションの回数
const int MAX_ITER = 100;  // 繰り返し計算の上限
const double CONVERGE_THRES = 1e-3;  // 収束判定用
const int CONVERGE_COUNT_THRES = 1;  // 収束判定用
const int T_BAR = 3;  // 時間帯上限
const double DELTA_T = 1;  // 離散的な時刻の間隔
const double A = 1;
const double B = 2;  // 鉄道リンクのBPR関数の係数
const double D = 0.8;
const double E = 1.6;  // ラチ内リンクのBPR関数の係数
const double BETA_H = 1;  // 乗車時間の時間価値
const double CAP = 5;  // 列車の定員
const double CAP_S = 5;  // ラチ内の定員
const double M_MAX = 200;  // 1日の使用上限
const double M0 = 1;  // 使用できるポイントの1単位
const double C = 0.3;  // 場所の効用関数のパラメータ
const double C2 = 0.5;  // 場所の効用関数のパラメータその2
const double C_BOARD = 1;  // 乗車コスト
const double DYNAMICS_STEP = 0.00001;  // ダイナミクスのステップ幅
const double GAMMA = 0.7;  // 1ポイントの金銭的価値
const double U0_AFF = 35;
const double U0_NON = 35;
const double RHO = 0;
const int MAX_N = 100;
const double P_AFF = 5;  // 系列店での財1個の価格
const double P_NON = 4.8;  // 非系列店での財1個の価格
const double MU = 5;  // ロジットモデルのスケールパラメータ
const double DELTA_P = 0.5;  // ポイント付与額計算時のステップ幅
const std::string DIR = "/home/suzuki/master/point3/";
// const std::string SUBDIR = "61/";
const std::string NODE_FNAME = "node.csv";
const std::string LINK_FNAME = "link.csv";
const std::string PATH_FNAME = "path.csv";
const std::string DEMAND_FNAME = "demand.csv";
const std::string SUMCOST_FNAME = "sumCostTrans.csv";
const std::string RAILCOST_FNAME = "railCostTrans.csv";
const std::string STAYUTIL_FNAME = "stayUtilTrans.csv";
const std::string PATHCOST_FNAME = "pathCostTrans.csv";
const std::string POSSESSED_FNAME = "possessedTrans.csv";
const std::string LINKFLOW_FNAME = "linkFlowTrans.csv";
const std::string ZEROFLOW_FNAME = "zeroFlowTrans.csv";
const std::string ONEFLOW_FNAME = "oneFlowTrans.csv";
const std::string PATHFLOW_FNAME = "pathFlowTrans.csv";
const std::string MONEY_FNAME = "moneyTrans.csv";
const std::string POINT_FNAME = "pointTrans.csv";
const std::string RAILREV_FNAME = "railRevenueTrans.csv";
const std::string STOREREV_FNAME = "storeRevenueTrans.csv";
const std::string PARAM_FNAME = "parameters.csv";
const std::string PERTURB_FNAME = "perturbation.csv";
const std::string ZEROCOST_FNAME = "zeroCostTrans.csv";
const std::string ONECOST_FNAME = "oneCostTrans.csv";
const std::string CONTOUR_FNAME = "contour.csv";
const std::string CONTOUR_OBJ_FNAME = "contourObj.csv";
const std::string CONTOUR_VEC_FNAME = "contourVec.csv";
const std::string STREAM_FNAME = "stream.csv";
const std::string STREAM_VEC_FNAME = "streamVec.csv";
const std::vector<int> POI_02 = {0, 1, 2};
const std::vector<int> POI_01 = {9, 11, 14};


void Link::CalcLinkFlow(std::vector<Path> pathVec)
{
    int i, j;

    this->x = 0;
    this->xFromZero = 0;
    this->xFromOne = 0;
    for (auto &&idx : this->inPathIdx) {
        this->x += pathVec[idx].f;
        if (pathVec[idx].odidx == 0) {
            this->xFromZero += pathVec[idx].f;
        }
        if (pathVec[idx].odidx == 1) {
            this->xFromOne += pathVec[idx].f;
        }
    }
}


void Link::CalcRailLinkCost(std::function<double(double, double)> RailLinkCostFunc, std::function<double(double, double)> StationLinkCostFunc, std::vector<Link> linkVec)
{
    if (this->type == 'r') {
        this->linkCost = RailLinkCostFunc(this->x, DELTA_T);
    } else if (this->type == 'i') {
        double flow = this->x;
        for (auto &&idx : this->pairIdx) {
            flow += linkVec[idx].x;
        }
        this->linkCost = StationLinkCostFunc(flow, DELTA_T);
    } else if (this->type != 'a') {
        this->linkCost = RailLinkCostFunc(0.0, DELTA_T);
    }
}


void Link::CalcRailLinkCostPrime(std::function<double(double, double)> LinkCostFuncPrime, std::function<double(double, double)> StationLinkCostFuncPrime, std::vector<Link> linkVec)
{
    if (this->type == 'r') {
        this->linkCostPrime = LinkCostFuncPrime(this->x, DELTA_T);
    } else if (this->type == 'i') {
        double flow = this->x;
        for (auto &&idx : this->pairIdx) {
            flow += linkVec[idx].x;
        }
        this->linkCost = StationLinkCostFuncPrime(flow, DELTA_T);
    }
}


void Link::CalcLinkPrice()
{
    if (this->type == 'r' || this->type == 'i') {
        this->price = this->linkCostPrime * this->x;
    }
}


// 未整備
void Link::CalcRailLinkPotential(std::function<double(double, double)> LinkCostFuncIntegral)
{
    if (this->type == 'r') {
        this->potential = LinkCostFuncIntegral(this->x, DELTA_T) - LinkCostFuncIntegral(0, DELTA_T);
    }
}


// 経路のコスト計算
void Path::CalcPathCost(std::vector<Link> linkVec, std::vector<Demand> demandVec, std::function<double(double, double)> StayLinkCostFunc, std::function<double(double, double)> StayLinkCostFunc2)
{
    double prev = this->pathCost;
    this->pathCost = 0;
    for (auto &&idx : this->linkSeries) {
        this->pathCost += linkVec[idx].linkCost;
        if (linkVec[idx].type == 's') {
            this->pathCost += StayLinkCostFunc(demandVec[this->odidx].B, linkVec[idx].poiU);  // 1つ目の滞在場所の効用
        } else if (linkVec[idx].type == 't') {
            this->pathCost += StayLinkCostFunc2(demandVec[this->odidx].B, linkVec[idx].poiU);  // 2つ目の滞在場所の効用
        } else {
            this->pathCost -= linkVec[idx].poiU;  // スケジューリングコスト
        }
    }
    this->pathCost += (this->boardNum - 1) * C_BOARD;
}


// 課金額を含む経路のコスト計算
void Path::CalcPathCostWithPrice(std::vector<Link> linkVec)
{
    this->pathCostWithPrice = this->pathCost;
    for (auto &&idx : this->linkSeries) {
        if (linkVec[idx].type == 'r') {
            this->pathCostWithPrice += linkVec[idx].price;
        }
    }
}


// 消費ポイント数計算（戻り値は鉄道会社の収益）
double Demand::CalcConsumedPoint(std::vector<Path> pathVec, std::vector<Link> linkVec)
{
    double deno, revenue;
    int n, l;
    bool isStay;

    this->L = 0;
    revenue = 0;
    for (auto &&idx : this->pathIdx) {
        isStay = false;
        for (auto &&linkIdx : pathVec[idx].linkSeries) {
            if (linkVec[linkIdx].type == 's' || (linkVec[linkIdx].type == 't' && C2 != 0)) {
                isStay = true;
                break;
            }
        }
        if (false) {
            continue;
        }
            
        // ロジットモデルの分母計算
        deno = 0;
        for (n = 0; n <= MAX_N; n++) {
            for (l = 0; l * M0 <= std::min(this->B, P_AFF * n); l++) {
                deno += std::exp(UtilityByAff(this->B, n, l) / MU);
                // std::cout << UtilityByAff(n, l) / MU << " ";
            }
        }
        for (n = 0; n <= MAX_N; n++) {
            deno += std::exp(UtilityByNon(n) / MU);
        }
        // std::cout << deno << " ";

        // 消費ポイント数の合計値・鉄道会社の収益計算
        for (n = 0; n <= MAX_N; n++) {
            for (l = 0; l * M0 <= std::min(this->B, P_AFF * n); l++) {
                this->L += (l * M0) * pathVec[idx].f * (std::exp(UtilityByAff(this->B, n, l) / MU) / deno);
                revenue += pathVec[idx].f * (P_AFF * n - l * M0) * (std::exp(UtilityByAff(this->B, n, l) / MU) / deno);
            }
        }
    }

    return revenue;
}


// 鉄道リンクのリンクコスト関数
double RailLinkCostFunc(double u, double t)
{
    return BETA_H * (1 + A * std::pow(u / CAP, B)) * t;
}


// 鉄道リンクのリンクコスト関数の微分
double RailLinkCostFuncPrime(double u, double t)
{
    return BETA_H * A * B * std::pow(u / CAP, B - 1) * t / CAP;
}


// 鉄道リンクのリンクコスト関数の不定積分
double RailLinkCostFuncIntegral(double u, double t)
{
    return BETA_H * (u + (A * CAP / (B + 1)) * std::pow(u / CAP, B + 1)) * t;
}


// 鉄道リンクのリンクコスト関数
double StationLinkCostFunc(double u, double t)
{
    return BETA_H * (1 + D * std::pow(u / CAP_S, E)) * t;
}


// 鉄道リンクのリンクコスト関数の微分
double StationLinkCostFuncPrime(double u, double t)
{
    return BETA_H * D * E * std::pow(u / CAP_S, E - 1) * t / CAP_S;
}


// 鉄道リンクのリンクコスト関数の不定積分
double StationLinkCostFuncIntegral(double u, double t)
{
    return BETA_H * (u + (D * CAP_S / (E + 1)) * std::pow(u / CAP_S, E + 1)) * t;
}


// 滞在リンクのリンクコスト関数
double StayLinkCostFunc(double b, double poiU)
{
    if (b < M_MAX) {
        return -C * std::sqrt(b) - poiU;
    } else {
        return -C * std::sqrt(M_MAX) - poiU;
    }
}


// 滞在リンクのリンクコスト関数その2
double StayLinkCostFunc2(double b, double poiU)
{
    if (b < M_MAX) {
        return -C2 * std::sqrt(b) - poiU;
    } else {
        return -C2 * std::sqrt(M_MAX) - poiU;
    }
}

// 財n個から得られる効用
double UtilityByGoods(double U0, int n)
{
    return U0 * std::sqrt(n);
}

// Bポイント持っている人がP円の財を購入するときにポイントをl単位消費することのコスト
double PointConsumeCost(double B, double P, int l)
{
    return (P > 0) ? (GAMMA * l * M0 + RHO * ((l * M0) / (P)) * ((l * M0) / (P))) : 0;
}


// Bポイント保有している人が系列店で財n個を購入し，ポイントをl単位消費したときの効用
double UtilityByAff(double B, int n, int l)
{
    return UtilityByGoods(U0_AFF, n) - (n * P_AFF - l * M0) - PointConsumeCost(B, n * P_AFF, l);
}


// 非系列店で財n個を購入ししたときの効用
double UtilityByNon(int n)
{
    return UtilityByGoods(U0_NON, n) - n * P_NON;
}