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

extern const int MAX_ITER;  // 繰り返し計算の上限
extern const double CONVERGE_THRES;  // 収束判定用
extern const int CONVERGE_COUNT_THRES;  // 収束判定用
extern const double MAX_NORM;  // 勾配の最大値
extern const double CAP_COMP;  // BPR関数の容量は定員の何分の1か
extern const double SPAN;  // 列車の運行間隔
extern const double NU1; // 総コスト最小化ステップのバリアパラメータ
extern const double NU2;  // 待ち行列除去ステップのパラメータ
extern const int T_BAR;  // 時間帯上限
extern const int T_STAR;  // 希望到着時刻
extern const double A;
extern const double B;  // BPR関数の係数
extern const double REMAIN_COST;  // 移動失敗者のコスト
extern const double BETA_W;  // 待ち時間の時間価値
extern const double BETA_H;  // 乗車時間の時間価値
extern const double BETA_E;  // 早着に関する時間価値
extern const double BETA_L;  // 遅着に関する時間価値
extern const double DELTA;  // 微小時間
extern const double CAP;  // 列車の定員
extern const std::string DIR;
extern const std::string SUBDIR;
extern const std::string NW_FNAME;
extern const std::string POP_FNAME;
extern const std::string SUMCOST_FNAME;
extern const std::string X_FNAME;
extern const std::string OVER_FNAME;
extern const std::string C_FNAME;
extern const std::string PARAM_FNAME;


// 空間的なリンクを持つ構造体(時刻・リンクごとのベクターとして持つ)
struct SpatialLink {
    int spaceIdx;
    int timeIdx;
    int o;
    int d;
    double t;  // 所要時間（t_phi）
    double u = 0;  // 利用者数
    double cost = 0;  // コスト関数
    double cost_prime = 0;  // コストの微分値
    double price = 0;  // 課金額


    // 利用者数を計算
    void CalcUser(std::vector<std::vector<double>> x);

    // コスト関数を計算
    void CalcCost(std::function<double(double, double)> LinkCostFunc);

    // コスト関数の微分値を計算
    void CalcCostPrime(std::function<double(double, double)> LinkCostFuncPrime);

    // 課金額を計算
    void CalcLinkPrice();
};


// 集団に関する情報を持つ構造体
struct Population {
    int id;
    int o;
    int d;
    int m;  // マス
};

// スケジューリングコスト
double DelayCost(int t);

// リンクコスト関数
double LinkCostFunc(double u, double t);

// リンクコスト関数の微分
double LinkCostFuncPrime(double u, double t);

#endif