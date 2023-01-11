#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_

#include <vector>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

extern const int MAX_ITER;  // 繰り返し計算の上限
extern const double CONVERGE_THRES;  // 収束判定用
extern const int CONVERGE_COUNT_THRES;  // 収束判定用
extern const double MAX_NORM;  // 勾配の最大値
extern const double CAP_COMP;  // BPR関数の容量は定員の何分の1か
extern const double SPAN;  // 列車の運行間隔
extern const double NU1; // 総コスト最小化ステップのバリアパラメータ
extern const double NU2;  // 待ち行列除去ステップのパラメータ
extern const double KAPPA;  // 総コスト最小化ステップのパラメータ
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
extern const int ACTION_NUM;  // 行動数（商品数）
extern const std::vector<double> U_COM;  // 商品から得られる効用
extern const std::vector<double> P;  // 価格
extern const double BETA_T;  // 価値関数の時間割引率
extern const double R;  // 店Aのポイント還元率
extern const int K_THRES;  // 価値反復1回あたりのサンプリング回数
extern const double MAX_POINT;  // ポイントの上限値（下限値は0で良い）
extern const double VALUE_THRES;  // 価値反復の収束判定閾値
extern const double ALPHA;  // 学習率
extern const double GAMMA;  // ポイントの相対的価値
extern const int BOOTSTRAP_N;  // ブートストラップのサンプリング回数¥
extern const int BASE_NUM;  // 基底関数の数
extern const double MAX_SCALE;  // スケーリング用
extern const double B0_SCALE;
extern const double B1_SCALE;
extern const double B2_SCALE;
extern const double B3_SCALE;
extern const int MONTE_NUM;  // モンテカルロシミュレーションのシミュレーション回数
extern const int MONTE_DUR;  // モンテカルロシミュレーションの期間
extern const double CHEAP;  // 店Bは店Aに比べてどれくらい安いか
extern const double MU;  // RLモデルのスケールパラメータ
extern const std::vector<std::function<double(double)>> BASE_FUNC;  // 基底関数
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

// プロスペクト理論の価値関数
double ProspectValue(double x);

#endif