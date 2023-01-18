#include "output.hpp"


// 1次元ベクター出力用
void Write1DFile(std::vector<double> vec, std::string fName, std::string colName)
{
    int i;
    std::ofstream ofs(fName);

    // 列名
    ofs << "step," << colName << std::endl;

    // 入力
    for (i = 0; i < static_cast<int>(vec.size()); i++) {
        ofs << i << "," << vec[i] << std::endl;
    }
}


// 2次元ベクター出力用
void Write2DFile(std::vector<std::vector<double>> vec, std::string fName)
{
    int i, j;
    std::ofstream ofs(fName);

    // 列名
    ofs << "step,";
    for (j = 0; j < static_cast<int>(vec[0].size()) - 1; j++) {
        ofs << j << ",";
    }
    ofs << static_cast<int>(vec[0].size()) - 1 << std::endl;

    // 入力
    for (i = 0; i < static_cast<int>(vec.size()); i++) {
        ofs << i << ",";
        for (j = 0; j < static_cast<int>(vec[i].size()) - 1; j++) {
            ofs << vec[i][j] << ",";
        }
        ofs << vec[i][vec[i].size() - 1] << std::endl;
    }
}


void WriteParameterFile(std::string SUBDIR)
{
    std::ofstream ofs(DIR + SUBDIR + PARAM_FNAME);

    ofs << "SIM_NUM," << SIM_NUM << std::endl;  // シミュレーション回数
    ofs << "MAX_ITER," << MAX_ITER << std::endl;  // 繰り返し計算の上限
    ofs << "CONVERGE_THRES," << CONVERGE_THRES << std::endl;  // 収束判定用
    ofs << "CONVERGE_COUNT_THRES," << CONVERGE_COUNT_THRES << std::endl;  // 収束判定用
    ofs << "T_BAR," << T_BAR << std::endl;  // 時間帯上限
    ofs << "DELTA_T," << DELTA_T << std::endl;  // 離散的な時刻の間隔
    ofs << "A," << A << std::endl;
    ofs << "B," << B << std::endl;  // BPR関数の係数
    ofs << "BETA_H," << BETA_H << std::endl;  // 乗車時間の時間価値
    ofs << "CAP," << CAP << std::endl;  // 列車の定員
    ofs << "M_MAX," << M_MAX << std::endl;  // 1日の使用上限
    ofs << "M0," << M0 << std::endl;  // 使用できるポイントの1単位
    ofs << "C," << C << std::endl;  // 場所の効用関数のパラメータ
    ofs << "C2," << C2 << std::endl;  // 場所の効用関数のパラメータその2
    ofs << "C_BOARD," <<  C_BOARD << std::endl;  // 乗車コスト
    ofs << "DYNAMIC_STEP," << DYNAMICS_STEP << std::endl;  // ダイナミクスのステップ幅
    ofs << "GAMMA," << GAMMA << std::endl;  // 1ポイントの金銭的価値
    ofs << "U0_AFF," << U0_AFF << std::endl;
    ofs << "U0_NON," << U0_NON << std::endl;
    ofs << "RHO," << RHO << std::endl;
    ofs << "MAX_N," << MAX_N << std::endl;
    ofs << "P_AFF," << P_AFF << std::endl;  // 系列店での財1個の価格
    ofs << "P_NON," << P_NON << std::endl;  // 非系列店での財1個の価格
    ofs << "MU," << MU << std::endl;  // ロジットモデルのスケールパラメータ
    ofs << "DELTA_P" << DELTA_P << std::endl;  // ポイント付与額計算時のステップ幅
}