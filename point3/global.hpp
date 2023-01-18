#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <vector>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <map>
#include <chrono>

// extern const int MODE;  // 均衡解スタート1回（0） or 相図標高線用（1）or 相図流線用 (2)
extern const bool PATH_MODE;  // パスベースの変数を記録するか否か（メモリ圧迫防止）
extern const int SIM_NUM;  // シミュレーションの回数
extern const int MAX_ITER;  // 繰り返し計算の上限
extern const double CONVERGE_THRES;  // 収束判定用
extern const int CONVERGE_COUNT_THRES;  // 収束判定用
extern const int T_BAR;  // 時間帯上限
extern const double DELTA_T;  // 時間の間隔
extern const double A;
extern const double B;  // 鉄道リンクのBPR関数の係数
extern const double D;
extern const double E;  // ラチ内リンクのBPR関数の係数
extern const double BETA_H;  // 乗車時間の時間価値
extern const double CAP;  // 列車の定員
extern const double CAP_S;  // ラチ内の定員
extern const double M_MAX;  // 1日に使用できるポイントの上限額
extern const double M0;  // 使用できるポイントの1単位
extern const double C;  // 場所の効用関数のパラメータ
extern const double C2; // 場所の効用関数のパラメータその2
extern const double C_BOARD;  // 乗車コスト
extern const double DYNAMICS_STEP;  // ダイナミクスのステップ幅
extern const double GAMMA;  // 1ポイントの金銭的価値
extern const double U0_AFF;  // 系列店の1単位目の財から得られる効用
extern const double U0_NON;  // 非系列店1単位目の財から得られる効用
extern const double RHO;  // ポイント消費コストのパラメータ
extern const int MAX_N;  // 1回の買い物での最大購入数
extern const double P_AFF;  // 系列店の財1個あたりの価格
extern const double P_NON;  // 非系列店の財1個あたりの価格
extern const double MU;  // ロジットモデルのスケールパラメータ
extern const double DELTA_P;  // ポイント付与額計算時のステップ幅
extern const std::string DIR;
// extern const std::string SUBDIR;
extern const std::string NODE_FNAME;
extern const std::string LINK_FNAME;
extern const std::string PATH_FNAME;
extern const std::string DEMAND_FNAME;
extern const std::string SUMCOST_FNAME;
extern const std::string RAILCOST_FNAME;
extern const std::string STAYUTIL_FNAME;
extern const std::string PATHCOST_FNAME;
extern const std::string POSSESSED_FNAME;
extern const std::string LINKFLOW_FNAME;
extern const std::string ZEROFLOW_FNAME;
extern const std::string ONEFLOW_FNAME;
extern const std::string PATHFLOW_FNAME;
extern const std::string MONEY_FNAME;
extern const std::string POINT_FNAME;
extern const std::string RAILREV_FNAME;
extern const std::string STOREREV_FNAME;
extern const std::string PARAM_FNAME;
extern const std::string PERTURB_FNAME;
extern const std::string ZEROCOST_FNAME;
extern const std::string ONECOST_FNAME;
extern const std::string CONTOUR_FNAME;
extern const std::string CONTOUR_OBJ_FNAME;
extern const std::string CONTOUR_VEC_FNAME;
extern const std::string STREAM_FNAME;
extern const std::string STREAM_VEC_FNAME;
extern const std::vector<int> POI_01;
extern const std::vector<int> POI_02;

struct Node;
struct Link;
struct Path;
struct Demand;

// ノードを持つ構造体
struct Node {
    int idx;  // ベクターのindex
    int i;  // 空間的なノード番号
    int t;  // 時刻
    char type;  // タイプ（s：駅ノード，g：発生ノード，a：吸収ノード）
};

// リンクを持つ構造体(時刻・リンクごとのベクターとして持つ)
struct Link {
    int idx;  // ベクターのindex
    int oidx;  // 始点ノードのidx
    int didx;  // 終点ノードのidx
    char type;  // タイプ（r：鉄道リンク，s：滞在リンク，a：吸収・自宅滞在リンク，i：ラチ内リンク，o：その他のリンク）
    double x = 0;  // フロー
    double xFromZero = 0;  // ODパターン0由来のフロー
    double xFromOne = 0;  // ODパターン1由来のフロー
    double linkCost = 0;  // コスト関数
    double linkCostPrime = 0;  // コストの微分値
    double potential = 0;  // リンクコスト関数の積分値
    double price = 0;  // 課金額
    double money = 0;  // 金銭での課金額
    double point = 0;  // ポイント付与額
    double poiU;  // 保有ポイント数0のときの滞在利得
    std::vector<int> inPathIdx;  // 含まれているパスのindex集合
    std::vector<int> pairIdx;  // 同じラチ内を表すリンクのindex（type == 'i'のときのみ有効）

    // 利用者数を計算
    void CalcLinkFlow(std::vector<Path> pathVec);

    // リンクコストを計算
    void CalcRailLinkCost(std::function<double(double, double)> LinkCostFunc, std::function<double(double, double)> StationLinkCostFunc, std::vector<Link> linkVec);

    // コスト関数の微分値を計算
    void CalcRailLinkCostPrime(std::function<double(double, double)> LinkCostFuncPrime, std::function<double(double, double)> StationLinkCostFuncPrime, std::vector<Link> linkVec);

    // 課金額を計算
    void CalcLinkPrice();

    // ポテンシャルの計算
    void CalcRailLinkPotential(std::function<double(double, double)> LinkCostFuncIntegral);
};


// 活動経路に関する情報をもつ構造体
struct Path {
    int idx;  // ベクターのindex
    int odidx;  // 対応するODのindex
    std::vector<int> linkSeries;  // リンクのindexの系列
    int boardNum;  // 列車への乗車回数
    double f;  // 経路フロー
    double pathCost;  // 経路コスト
    double pathCostWithPrice;  // 課金額を含む経路コスト

    // コストを計算
    void CalcPathCost(std::vector<Link> linkVec, std::vector<Demand> demandVec, std::function<double(double, double)> StayLinkCostFunc, std::function<double(double, double)> StayLinkCostFunc2);

    // 課金額を含むコストを計算
    void CalcPathCostWithPrice(std::vector<Link> linkVec);
};


// 需要に関する情報を持つ構造体
struct Demand {
    int idx;
    int o;
    int d;
    int m;  // マス
    double B = 0;  // 保有ポイント数
    double L = 0;  // 消費ポイント数
    std::vector<int> pathIdx;  // 活動経路の選択肢集合

    // 消費ポイント数計算
    double CalcConsumedPoint(std::vector<Path> pathVec, std::vector<Link> linkVec);
};

// 鉄道リンクのリンクコスト関数
double RailLinkCostFunc(double u, double t);

// 鉄道リンクのリンクコスト関数の微分
double RailLinkCostFuncPrime(double u, double t);

// 鉄道リンクのリンクコスト関数の積分
double RailLinkCostFuncIntegral(double u, double t);

// ラチ内リンクのリンクコスト関数
double StationLinkCostFunc(double u, double t);

// ラチ内リンクのリンクコスト関数の微分
double StationLinkCostFuncPrime(double u, double t);

// ラチ内リンクのリンクコスト関数の積分
double StationLinkCostFuncIntegral(double u, double t);

// 滞在リンクのリンクコスト関数
double StayLinkCostFunc(double b, double poiU);

// 滞在リンクのリンクコスト関数その2
double StayLinkCostFunc2(double b, double poiU);

// 財n個から得られる効用
double UtilityByGoods(double U0, int n);

// P円の財を購入するときにポイントをl単位消費することのコスト
double PointConsumeCost(double B, double P, int l);

// 系列店で財n個を購入し，ポイントをl単位消費したときの効用
double UtilityByAff(double B, int n, int l);

// 非系列店で財n個を購入したときの効用
double UtilityByNon(int n);

#endif