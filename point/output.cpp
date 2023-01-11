#include "output.hpp"


void WriteSumCostFile(std::vector<double> sumCostTrans, std::string sumCostName)
{
    int i;

    std::ofstream ofs(sumCostName);

    ofs << "step,sumcost," << std::endl;

    for (i = 0; i < static_cast<int>(sumCostTrans.size()); i++) {
        ofs << i << "," << sumCostTrans[i] << "," << std::endl;
    }
}


void WriteXCTransFile(std::vector<std::vector<std::vector<double>>> xTrans, std::vector<std::vector<std::vector<double>>> cTrans, std::string xTransName, std::string cTransName)
{
    int i, j, t;

    std::ofstream ofs_x(xTransName), ofs_c(cTransName);

    ofs_x << "step,";
    ofs_c << "step,";
    for (t = 0; t <= T_BAR; t++) {
        for (i = 0; i < static_cast<int>(xTrans[0][0].size()); i++) {
            ofs_x << "x_" << t << i << ",";
            ofs_c << "c_" << t << i << ",";
        }
    }
    ofs_x << std::endl;
    ofs_c << std::endl;

    for (i = 0; i < static_cast<int>(xTrans.size()); i++) {
        ofs_x << i << ",";
        ofs_c << i << ",";
        for (t = 0; t <= T_BAR; t++) {
            for (j = 0; j < static_cast<int>(xTrans[0][0].size()); j++) {
                ofs_x << xTrans[i][t][j] << ",";
                ofs_c << cTrans[i][t][j] << ",";
            }
        }
        ofs_x << std::endl;
        ofs_c << std::endl;
    }
}

void WriteOverTransFile(std::vector<bool> overTrans, std::string overTransName)
{
    int i;

    std::ofstream ofs(overTransName);

    ofs << "step,is_over," << std::endl;

    for (i = 0; i < static_cast<int>(overTrans.size()); i++) {
        ofs << i << "," << overTrans[i] << "," << std::endl;
    }
}


void WriteParameterFile()
{
    std::ofstream ofs(DIR + SUBDIR + PARAM_FNAME);

    ofs << "MAX_ITER," << MAX_ITER << std::endl;  // 繰り返し計算の上限
    ofs << "CONVERGE_THRES," << CONVERGE_THRES << std::endl;  // 収束判定用
    ofs << "CONVERGE_COUNT_THRES," << CONVERGE_COUNT_THRES << std::endl;  // 収束判定用
    ofs << "MAX_NORM," << MAX_NORM << std::endl;  // 勾配の最大値
    ofs << "CAP_COMP," << CAP_COMP << std::endl;  // BPR関数の容量は定員の何分の1か
    ofs << "SPAN," << SPAN << std::endl;  // 列車の運行間隔
    ofs << "NU1," << NU1 << std::endl; // 総コスト最小化ステップのバリアパラメータ
    ofs << "NU2," << NU2 << std::endl;  // 待ち行列除去ステップのパラメータ
    ofs << "T_BAR," << T_BAR << std::endl;  // 時間帯上限
    ofs << "T_STAR," << T_STAR << std::endl;  // 希望到着時刻
    ofs << "A," << A << std::endl;
    ofs << "B," << B << std::endl;  // BPR関数の係数
    ofs << "REMAIN_COST," << REMAIN_COST << std::endl;  // 移動失敗者のコスト
    ofs << "BETA_W," << BETA_W << std::endl;  // 待ち時間の時間価値
    ofs << "BETA_H," << BETA_H << std::endl;  // 乗車時間の時間価値
    ofs << "BETA_E," << BETA_E << std::endl;  // 早着に関する時間価値
    ofs << "BETA_L," << BETA_L << std::endl;  // 遅着に関する時間価値
    ofs << "DELTA," << DELTA << std::endl;  // 微小時間
    ofs << "CAP," << CAP << std::endl;  // 列車の定員
}