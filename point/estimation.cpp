#include "estimation.hpp"


// 近似的価値反復
std::vector<double> ApproxValueIteration()
{
    std::vector<double> b(BASE_NUM), prev(BASE_NUM), bEst(BASE_NUM, 0);
    int k, a, count, i, j, l;
    double prevDelta, delta, s, expSum, vTmp, used, vApprox, scale;

    std::random_device rnd;
    std::mt19937 mt(rnd());
    std::uniform_real_distribution<> rand(0, MAX_POINT);

    std::ofstream ofs(DIR + "params.csv");

    for (i = 0; i < BOOTSTRAP_N; i++) {
        if (i % (BOOTSTRAP_N / 10) == 0) {
            std::cout << i + 1 << "回目開始" << std::endl;
        }

        delta = 1e9;
        b[0] = 1;
        b[1] = 1;
        b[2] = 1;
        b[3] = 1;

        while (true) {
            std::cout << delta << std::endl;
            std::cout << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << std::endl;
            prevDelta = delta;
            delta = 0.0;
            k = 0;
            while (k < K_THRES) {
                s = rand(mt);  // sのサンプリング

                // 期待最大効用計算の際のスケーリングパラメータの計算
                scale = 0.0;
                for (j = 0; j < ACTION_NUM; j++) {  // 店A・ポイント不使用
                    if (s + R * P[j] > MAX_POINT) {
                        vTmp = b[0] * B0_SCALE + (b[1] * B1_SCALE) * std::tanh(B2_SCALE * b[2] * MAX_POINT - B3_SCALE * b[3]);
                        scale = std::max((1 / MU) * (U_COM[j] + ProspectValue(-P[j]) + ProspectValue(GAMMA * (MAX_POINT - s)) + BETA_T * vTmp) - MAX_SCALE, scale);
                    } else {
                        vTmp = b[0] * B0_SCALE + (b[1] * B1_SCALE) * std::tanh(B2_SCALE * b[2] * (s + R * P[j]) - B3_SCALE * b[3]);
                        scale = std::max((1 / MU) * (U_COM[j] + ProspectValue(-P[j]) + ProspectValue(GAMMA * (R * P[j])) + BETA_T * vTmp) - MAX_SCALE, scale);
                    }
                }
                for (j = 0; j < ACTION_NUM; j++) {  // 店A・ポイント使用
                    used = std::min(s, P[j]);
                    vTmp = b[0] * B0_SCALE + (b[1] * B1_SCALE) * std::tanh(B2_SCALE * b[2] * (s + R * P[j] - used) - B3_SCALE * b[3]);
                    scale = std::max((1 / MU) * (U_COM[j] + ProspectValue(-P[j] + used) + ProspectValue(GAMMA * (R * P[j] - used)) + BETA_T * vTmp) - MAX_SCALE, scale);
                }
                for (j = 0; j < ACTION_NUM; j++) {  // 店B・ポイント不使用
                    vTmp = b[0] * B0_SCALE + (b[1] * B1_SCALE) * std::tanh(B2_SCALE * b[2] * s - B3_SCALE * b[3]);
                    scale = std::max((1 / MU) * (U_COM[j] + ProspectValue(-P[j] * CHEAP) + BETA_T * vTmp) - MAX_SCALE, scale);
                }

                // 期待効用関数の中のサムの計算（店A・ポイント不使用）
                expSum = 0.0;
                for (j = 0; j < ACTION_NUM; j++) {
                    if (s + R * P[j] > MAX_POINT) {
                        vTmp = b[0] * B0_SCALE + (b[1] * B1_SCALE) * std::tanh(B2_SCALE * b[2] * MAX_POINT - B3_SCALE * b[3]);
                        expSum += std::exp((1 / MU) * (U_COM[j] + ProspectValue(-P[j]) + ProspectValue(GAMMA * (MAX_POINT - s)) + BETA_T * vTmp) - scale);
                    } else {
                        vTmp = b[0] * B0_SCALE + (b[1] * B1_SCALE) * std::tanh(B2_SCALE * b[2] * (s + R * P[j]) - B3_SCALE * b[3]);
                        expSum += std::exp((1 / MU) * (U_COM[j] + ProspectValue(-P[j]) + ProspectValue(GAMMA * (R * P[j])) + BETA_T * vTmp) - scale);
                    }
                }

                // 期待最大効用の中のサムの計算（店A・ポイント使用）
                for (j = 0; j < ACTION_NUM; j++) {
                    used = std::min(s, P[j]);
                    vTmp = b[0] * B0_SCALE + (b[1] * B1_SCALE) * std::tanh(B2_SCALE * b[2] * (s + R * P[j] - used) - B3_SCALE * b[3]);
                    expSum += std::exp((1 / MU) * (U_COM[j] + ProspectValue(-P[j] + used) + ProspectValue(GAMMA * (R * P[j] - used)) + BETA_T * vTmp) - scale);
                }

                // 期待最大効用の中のサムの計算（店B・ポイント不使用）
                for (j = 0; j < ACTION_NUM; j++) {
                    vTmp = b[0] * B0_SCALE + (b[1] + B1_SCALE) * std::tanh(B2_SCALE * b[2] * s - B3_SCALE * b[3]);
                    expSum += std::exp((1 / MU) * (U_COM[j] + ProspectValue(-P[j] * CHEAP) + BETA_T * vTmp) - scale);
                }

                // パラメータ更新
                vApprox = b[0] * B0_SCALE + (b[1] * B1_SCALE) * std::tanh(B2_SCALE * b[2] * s - B3_SCALE * b[3]);
                prev[0] = b[0];
                b[0] = b[0] + ALPHA * (MU * (scale + std::log(expSum)) - vApprox) * B0_SCALE;
                delta = std::max(delta, std::fabs(b[0] - prev[0]));
                prev[1] = b[1];
                b[1] = b[1] + ALPHA * (MU * (scale + std::log(expSum)) - vApprox) * B1_SCALE * std::tanh(B2_SCALE * b[2] * s - B3_SCALE * b[3]);
                delta = std::max(delta, std::fabs(b[1] - prev[1]));
                prev[2] = b[2];
                b[2] = b[2] + ALPHA * (MU * (scale + std::log(expSum)) - vApprox) * ((B1_SCALE * B2_SCALE * prev[1] * s) / (std::cosh(B2_SCALE * prev[2] * s - B3_SCALE * b[3]) * std::cosh(B2_SCALE * prev[2] * s - B3_SCALE * b[3])));
                delta = std::max(delta, std::fabs(b[2] - prev[2]));
                prev[3] = b[3];
                b[3] = b[3] - ALPHA * (MU * (scale + std::log(expSum)) - vApprox) * ((B1_SCALE * B3_SCALE * prev[1]) / (std::cosh(B2_SCALE * prev[2] * s - B3_SCALE * b[3]) * std::cosh(B2_SCALE * prev[2] * s - B3_SCALE * b[3])));
                delta = std::max(delta, std::fabs(b[3] - prev[3]));

                k++;
            }

            if (std::fabs(delta - prevDelta) < VALUE_THRES) {
                break;
            }

            count++;
        }

        if (i % (BOOTSTRAP_N / 10) == 0) {
            for (j = 0; j < BASE_NUM; j++) {
                std::cout << b[j] << " ";
            }
            std::cout << std::endl;
        }

        for (j = 0; j < BASE_NUM; j++) {
            bEst[j] += b[j];
        }
    }

    for (i = 0; i < BASE_NUM; i++) {
        bEst[i] /= BOOTSTRAP_N;
        std::cout << bEst[i] << " ";
    }
    std::cout << std::endl;

    ofs << BASE_NUM << std::endl;
    for (i = 0; i < BASE_NUM; i++) {
        ofs << bEst[i] << std::endl;
    }

    return bEst;
}


// 価値反復
void ValueIteration()
{
    std::vector<double> V(MAX_POINT + 1, 0);
    double v, scale, used, expSum, delta = 1e9;
    int s, i;
    std::ofstream ofs(DIR + "value.csv");

    while (delta >= VALUE_THRES) {
        delta = 0;

        for (s = 0; s < MAX_POINT + 1; s++) {
            v = V[s];

            // スケーリング
            scale = 0.0;
            for (i = 0; i < ACTION_NUM; i++) {  // 店A・ポイント不使用
                if (s + R * P[i] > MAX_POINT) {
                    scale = std::max((1 / MU) * (U_COM[i] + ProspectValue(-P[i]) + ProspectValue(GAMMA * (MAX_POINT - s)) + BETA_T * V[MAX_POINT]) - MAX_SCALE, scale);
                } else {
                    scale = std::max((1 / MU) * (U_COM[i] + ProspectValue(-P[i]) + ProspectValue(GAMMA * (R * P[i])) + BETA_T * V[static_cast<int>(s + R * P[i])]) - MAX_SCALE, scale);
                }
            }
            for (i = 0; i < ACTION_NUM; i++) {  // 店A・ポイント使用
                used = std::min(s, static_cast<int>(P[i]));
                scale = std::max((1 / MU) * (U_COM[i] + ProspectValue(-P[i] + used) + ProspectValue(GAMMA * (R * P[i] - used)) + BETA_T * V[static_cast<int>(s + R * P[i] - used)]) - MAX_SCALE, scale);
            }
            for (i = 0; i < ACTION_NUM; i++) {  // 店B・ポイント不使用
                scale = std::max((1 / MU) * (U_COM[i] + ProspectValue(-P[i] * CHEAP) + BETA_T * V[static_cast<int>(s)]) - MAX_SCALE, scale);
            }

            // ログサム計算
            expSum = 0;
            for (i = 0; i < ACTION_NUM; i++) {  // 店A・ポイント不使用
                if (s + R * P[i] > MAX_POINT) {
                    expSum += std::exp((1 / MU) * (U_COM[i] + ProspectValue(-P[i]) + ProspectValue(GAMMA * (MAX_POINT - s)) + BETA_T * V[MAX_POINT]) - scale);
                } else {
                    expSum += std::exp((1 / MU) * (U_COM[i] + ProspectValue(-P[i]) + ProspectValue(GAMMA * (R * P[i])) + BETA_T * V[static_cast<int>(s + R * P[i])]) - scale);
                }
            }
            for (i = 0; i < ACTION_NUM; i++) {  // 店A・ポイント使用
                used = std::min(s, static_cast<int>(P[i]));
                expSum += std::exp((1 / MU) * (U_COM[i] + ProspectValue(-P[i] + used) + ProspectValue(GAMMA * (R * P[i] - used)) + BETA_T * V[static_cast<int>(s + R * P[i] - used)]) - scale);
            }
            for (i = 0; i < ACTION_NUM; i++) {  // 店B・ポイント不使用
                expSum += std::exp((1 / MU) * (U_COM[i] + ProspectValue(-P[i] * CHEAP) + BETA_T * V[static_cast<int>(s)]) - scale);
            }

            // 価値関数更新
            V[s] = MU * (log(expSum) + scale);
            delta = std::max(delta, std::fabs(V[s] - v));
            // std::cout << V[s] << std::endl;
        }
    }

    ofs << "points,value" << std::endl;
    for (s = 0; s < MAX_POINT + 1; s++) {
        ofs << s << "," << V[s] << std::endl;
    }
    std::cout << "書き込み完了" << std::endl;
}