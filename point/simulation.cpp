#include "simulation.hpp"

// 貨幣のポイント変換
double MoneyToPoint(double m, double s, std::vector<double> b)
{
    return (1 / (B2_SCALE * b[2])) * std::atanh(m / (B1_SCALE * b[1]) + std::tanh(B2_SCALE * b[2] * s - B3_SCALE * b[3])) - s + (B3_SCALE * b[3]) / (B2_SCALE * b[2]);
}

// 各戦略の含めたコストを計算
std::vector<std::vector<double>> CalcCostWithQueue(std::vector<std::vector<SpatialLink>> &Link, std::vector<std::vector<double>> x, std::function<double(int)> DelayCost, std::function<double(double, double)> LinkCostFunc, std::function<double(double, double)> LinkCostFuncPrime)
{
    std::vector<std::vector<double>> y(T_BAR + 1, std::vector<double>(Link[0].size(), 0));
    std::vector<std::vector<double>> E(T_BAR + 1, std::vector<double>(Link[0].size(), 0));
    std::vector<std::vector<double>> wbar(T_BAR + 1, std::vector<double>(Link[0].size(), 0));
    std::vector<std::vector<double>> gbar(T_BAR + 1, std::vector<double>(Link[0].size(), 0));
    std::vector<std::vector<double>> hbar(T_BAR + 1, std::vector<double>(Link[0].size(), 0));
    std::vector<std::vector<double>> c(T_BAR + 1, std::vector<double>(Link[0].size(), 0));
    std::vector<std::vector<std::vector<double>>> BP(T_BAR + 1, std::vector<std::vector<double>>(Link[0].size(), std::vector<double>(T_BAR + 2, 0)));
    int t, i, s, j;
    double remainCap, pathCost;
    bool flag;
    std::vector<double> accumX(T_BAR + 1), accumY(T_BAR + 1);

    // t = 0のとき
    remainCap = CAP;
    for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
        y[0][i] = ((x[0][i] > remainCap) ? remainCap : x[0][i]);
        E[0][i] = ((x[0][i] > remainCap) ? x[0][i] - remainCap : 0);
        remainCap -= y[0][i];
    }

    // t > 0のとき
    for (t = 1; t <= T_BAR; t++) {
        remainCap = CAP;
        for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
            y[t][i] = ((E[t - 1][i] + x[t][i] > remainCap) ? remainCap : E[t - 1][i] + x[t][i]);
            E[t][i] = E[t - 1][i] + ((E[t - 1][i] + x[t][i] > remainCap) ? x[t][i] - remainCap : -E[t - 1][i]);
            remainCap -= y[t][i];
        }
    }

    for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
        accumX[0] = x[0][i];
        accumY[0] = y[0][i];
        for (t = 1; t <= T_BAR; t++) {
            accumY[t] = accumY[t - 1] + y[t][i];
            accumX[t] = std::max(accumX[t - 1] + x[t][i], accumY[t]);
        }

        // t = 0のとき
        BP[0][i][0] = accumY[0] / accumX[0];
        BP[0][i][T_BAR + 1] = 1 - BP[0][i][0];
        for (s = 1; s <= T_BAR; s++) {
            if (accumY[s] <= accumX[0]) {
                BP[0][i][s] = (accumY[s] - accumY[s - 1]) / accumX[0];
                BP[0][i][T_BAR + 1] -= BP[0][i][s];
            } else {
                BP[0][i][s] = (accumX[0] - accumY[s - 1]) / accumX[0];
                BP[0][i][T_BAR + 1] -= BP[0][i][s];
                break;
            }
        }

        // t > 0のとき
        for (t = 1; t <= T_BAR; t++) {
            flag = true;
            BP[t][i][T_BAR + 1] = 1;
            for (s = 0; s <= T_BAR - t; s++) {
                if (accumX[t - 1] >= accumY[t + s]) {
                    continue;
                }
                
                if (flag) {
                    if (accumY[t + s] <= accumX[t]) {
                        BP[t][i][t + s] = (accumY[t + s] - accumX[t - 1]) / (accumX[t] - accumX[t - 1]);
                        BP[t][i][T_BAR + 1] -= BP[t][i][t + s];
                        flag = false;
                    } else {
                        BP[t][i][t + s] = 1;
                        BP[t][i][T_BAR + 1] -= BP[t][i][t + s];
                        break;
                    }
                } else {
                    if (accumY[t + s] <= accumX[t]) {
                        BP[t][i][t + s] = (accumY[t + s] - accumY[t + s - 1]) / (accumX[t] - accumX[t - 1]);
                        BP[t][i][T_BAR + 1] -= BP[t][i][t + s];
                    } else {
                        BP[t][i][t + s] = (accumX[t] - accumY[t + s - 1]) / (accumX[t] - accumX[t - 1]);
                        BP[t][i][T_BAR + 1] -= BP[t][i][t + s];
                        break;
                    }
                }
            }
        }
    }

    // リンクコストの計算
    for (t = 0; t <= T_BAR; t++) {
        for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
            Link[t][i].CalcUser(y);
            Link[t][i].CalcCost(LinkCostFunc);
            Link[t][i].CalcCostPrime(LinkCostFuncPrime);
            Link[t][i].CalcLinkPrice();
        }
    }

    for (t = 0; t <= T_BAR; t++) {
        for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
            for (s = t; s <= T_BAR; s++) {
                wbar[t][i] += (s - t + SPAN / 2) * BP[t][i][s];
                gbar[t][i] += DelayCost(s) * BP[t][i][s];
                
                pathCost = 0;
                for (j = i; j < static_cast<int>(Link[0].size()); j++) {
                    pathCost += Link[s][j].cost;
                }

                hbar[t][i] += pathCost * BP[t][i][s];
            }

            c[t][i] = BETA_W * wbar[t][i] + gbar[t][i] + hbar[t][i] + REMAIN_COST * BP[t][i][T_BAR + 1];
        }
    }

    return c;
}


// The Proportional-Switch Adjustment Process
// 現在の状態と利得関数を受け取って，微小時間後の状態を返す関数
std::vector<std::vector<double>> PSAP(std::vector<std::vector<double>> x, std::vector<Population> P, std::vector<std::vector<double>> c)
{
    double V;
    std::vector<std::vector<double>> x_after(x.size(), std::vector<double>(x[0].size()));
    int i, t, s;

    for (i = 0; i < static_cast<int>(P.size()); i++) {
        for (t = 0; t <= T_BAR; t++) {
            V = 0;
            for (s = 0; s <= T_BAR; s++) {
                V += (x[s][i] * std::max(c[s][i] - c[t][i], 0.0) - x[t][i] * std::max(c[t][i] - c[s][i], 0.0));
            }
            x_after[t][i] = x[t][i] + DELTA * V;
        }
    }

    return x_after;
}


std::vector<std::vector<double>> GradientClipping(std::vector<std::vector<double>> c)
{
    int t, i;
    double norm;
    bool overFlowFlag;
    std::vector<std::vector<double>> ret(c.size(), std::vector<double>(c[0].size()));

    norm = 0;
    overFlowFlag = false;
    for (t = 0; t <= T_BAR; t++) {
        for (i = 0; i < static_cast<int>(c[0].size()); i++) {
            if (std::isinf(c[t][i])) {
                std::cout << "infinity" << std::endl;
            }
            if (c[t][i] * c[t][i] < 0) {
                overFlowFlag = true;
                break;
            } else {
                norm += c[t][i] * c[t][i];
            }
        }
    }

    if (overFlowFlag || norm < 0) {
        norm = __DBL_MAX__;
    }
    norm = std::sqrt(norm);

    if (norm > MAX_NORM) {
        for (t = 0; t <= T_BAR; t++) {
            for (i = 0; i < static_cast<int>(c[0].size()); i++) {
                ret[t][i] = c[t][i] * MAX_NORM / norm;
            }
        }
    } else {
        ret = c;
    }

    return ret;
}


std::vector<std::vector<double>> OptimizeWithBarrier(std::vector<std::vector<SpatialLink>> Link, std::vector<std::vector<double>> init_x, std::vector<Population> P, std::function<double(int)> DelayCost, std::function<double(double, double)> LinkCostFunc, std::function<double(double, double)> LinkCostFuncPrime, std::vector<double> &sumCostTrans, std::vector<std::vector<std::vector<double>>> &xTrans, std::vector<bool> &overTrans, std::vector<std::vector<std::vector<double>>> &cTrans, std::vector<std::vector<double>> &pTrans, std::vector<double> b)
{
    std::vector<std::vector<double>> c(T_BAR + 1, std::vector<double>(Link[0].size(), 0)), x(T_BAR + 1, std::vector<double>(Link[0].size(), 0)), prev_x(T_BAR + 1, std::vector<double>(Link[0].size(), 0)), p(T_BAR + 1, std::vector<double>(Link[0].size(), 0));
    std::vector<double> pAccum(Link[0].size(), 0), xi(Link[0].size(), 0);
    double sumCost, diff;
    int t, i, j, counter, convergeCount;
    bool is_ok, is_strict_ok;

    counter = 0;
    x = init_x;
    for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
        for (t = 0; t <= T_BAR; t++) {
            xi[i] += x[t][i];
        }
    }
    while (true) {
        // コスト計算・リンクベースの課金額計算
        c = CalcCostWithQueue(Link, x, DelayCost, LinkCostFunc, LinkCostFuncPrime);
        std::vector<std::vector<double>> price(T_BAR + 1, std::vector<double>(Link[0].size(), 0));

        sumCost = 0;
        diff = 0;  // 収束確認用
        std::vector<double> arriveSum(T_BAR + 1, 0);
        is_ok = true;
        is_strict_ok = true;
        for (t = 0; t <= T_BAR; t++) {
            for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
                sumCost += (x[t][i] > 0 ? c[t][i] * x[t][i] : 0);
                arriveSum[t] += x[t][i];
                diff += std::abs(x[t][i] - prev_x[t][i]);
            }
            if (arriveSum[t] > CAP - 1e-6) {
                is_ok = false;
            }
            if (arriveSum[t] > CAP) {
                is_strict_ok = false;
            }
        }
        // if (is_ok) {
        //     for (t = 0; t <= T_BAR; t++) {
        //         sumCost -= NU1 * std::log(CAP - arriveSum[t]);
        //     }
        // }
        sumCostTrans.emplace_back(sumCost);
        xTrans.emplace_back(x);
        overTrans.emplace_back(!is_strict_ok);
        cTrans.emplace_back(c);

        if (counter % 1000 == 0) {
            std::cout << "ν = " << NU1 << "として最適化中：" << counter << "日経過" << std::endl;
        }
        counter++;

        // 収束していれば終了
        if (counter >= MAX_ITER) {
            break;
        }
        if (diff < CONVERGE_THRES) {
            convergeCount++;
            if (convergeCount >= CONVERGE_COUNT_THRES && is_ok) {
                break;
            }
        } else {
            convergeCount = 0;
        }

        // 戦略ベースの課金額計算・コストに足して次の状態に遷移
        if (is_ok) {
            for (t = 0; t <= T_BAR; t++) {
                for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
                    for (j = i; j < static_cast<int>(Link[0].size()); j++) {
                        price[t][i] += Link[t][j].price;
                    }
                    price[t][i] += (KAPPA - 1) * c[t][i];
                    price[t][i] += NU1 / (CAP - arriveSum[t]);
                    if (price[t][i] > 0) {
                        std::cout << "補助金ではなく課金になっています" << std::endl;
                        std::exit(EXIT_FAILURE);
                    }
                    p[t][i] = MoneyToPoint(-price[t][i], pAccum[i], b);
                }
            }
        } else {
            for (t = 0; t <= T_BAR; t++) {
                for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
                    // price[t][i] = exp(NU2 * (arriveSum[t] - CAP)) - c[t][i];
                    price[t][i] = std::max(2 * NU2 * (arriveSum[t] - CAP + 1e-6), 0.0) - c[t][i];
                    if (price[t][i] > 0) {
                        std::cout << "補助金ではなく課金になっています" << std::endl;
                        std::exit(EXIT_FAILURE);
                    }
                    p[t][i] = MoneyToPoint(-price[t][i], pAccum[i], b);
                }
            }
        }

        for (t = 0; t <= T_BAR; t++) {
            for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
                c[t][i] += price[t][i];
            }
        }
        
        // // 勾配クリッピング
        // c = GradientClipping(c);

        // 所有ポイント数更新
        std::vector<double> pi(Link[0].size(), 0);
        for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
            for (t = 0; t <= T_BAR; t++) {
                pi[i] += p[t][i];
            }
            pAccum[i] += pi[i] / xi[i];
        }
        pTrans.emplace_back(pAccum);

        prev_x = x;
        x = PSAP(x, P, c);
    }

    return x;
}

void SimulationWithout(std::vector<std::vector<SpatialLink>> Link, std::vector<std::vector<double>> init_x, std::vector<Population> P, std::function<double(int)> DelayCost, std::function<double(double, double)> LinkCostFunc, std::function<double(double, double)> LinkCostFuncPrime, std::vector<double> &sumCostTrans, std::vector<std::vector<std::vector<double>>> &xTrans)
{
    std::vector<std::vector<double>> c(T_BAR + 1, std::vector<double>(Link[0].size(), 0)), x(T_BAR + 1, std::vector<double>(Link[0].size(), 0));
    double nu1, nu2, sumCost;
    int counter, t, i;

    counter = 0;
    x = init_x;
    while (true) {
        c = CalcCostWithQueue(Link, x, DelayCost, LinkCostFunc, LinkCostFuncPrime);
        sumCost = 0;
        for (t = 0; t <= T_BAR; t++) {
            for (i = 0; i < static_cast<int>(Link[0].size()); i++) {
                sumCost += c[t][i];
            }
        }
        sumCostTrans.emplace_back(sumCost);
        xTrans.emplace_back(x);

        if (counter % 1000 == 0) {
            std::cout << "無課金で計算中：" << counter << "日経過" << std::endl;
        }
        counter++;

        if (counter >= MAX_ITER) {
            break;
        }
        x = PSAP(x, P, c);
    }
}


std::vector<std::vector<double>> makeInitDist(std::vector<Population> P)
{
    std::vector<std::vector<double>> x(T_BAR + 1, std::vector<double>(P.size(), 0));
    int i, t;
    std::vector<double> sum(P.size());

    for (i = 0; i < static_cast<int>(P.size()); i++) {
        x[T_STAR][i] = (P[i].m / T_BAR) * 1;
        sum[i] = P[i].m - x[T_STAR][i];
    }

    for (i = 0; i < static_cast<int>(P.size()); i++) {
        for (t = 0; t <= T_BAR; t++) {
            if (t == T_STAR) {
                continue;
            }
            x[t][i] = sum[i] / T_BAR;
        }
    }

    return x;
}


double ExpectedRevenue(double initS, std::vector<double> b)
{
    int i, t, a, selected;
    double s, prob, v, V, scale, deno, rn, revenueAvg;
    std::vector<double> nume(ACTION_NUM * 3);
    std::random_device rnd;
    std::mt19937 mt(rnd());
    std::uniform_real_distribution<> rand(0, 1);

    revenueAvg = 0.0;
    for (i = 0; i < MONTE_NUM; i++) {
        s = initS;
        for (t = 0; t < MONTE_DUR; t++) {
            scale = 0.0;
            deno = 0.0;
            // スケーリング（オーバーフロー防止）
            for (a = 0; a < ACTION_NUM; a++) {  // 店A・ポイント使用
                v = U_COM[a] - (P[a] - std::min(P[a], s)) - GAMMA * (std::min(P[a], s) - R * P[a]);  // 即時効用
                V = B0_SCALE * b[0] + B1_SCALE * b[1] * std::tanh(B2_SCALE * b[2] * (s - std::min(P[a], s) + R * P[a]) - B3_SCALE * b[3]);  // 期待最大効用
                nume[a] = (1 / MU) * (v + BETA_T * V);
                scale = std::max(nume[a] - MAX_SCALE, scale);
            }
            for (a = 0; a < ACTION_NUM; a++) {  // 店A・ポイント不使用
                v = U_COM[a] - P[a] + GAMMA * std::min(R * P[a], MAX_POINT - s);
                V = B0_SCALE * b[0] + B1_SCALE * b[1] * std::tanh(B2_SCALE * b[2] * (s + std::min(R * P[a], MAX_POINT - s)) - B3_SCALE * b[3]);
                nume[ACTION_NUM + a] = (1 / MU) * (v + BETA_T * V);
                scale = std::max(nume[ACTION_NUM + a] - MAX_SCALE, scale);
            }
            for (a = 0; a < ACTION_NUM; a++) {  // 店B・ポイント不使用
                v = U_COM[a] - P[a] * CHEAP;
                V = B0_SCALE * b[0] + B1_SCALE * b[1] * std::tanh(B2_SCALE * b[2] * s - B3_SCALE * b[3]);
                nume[2 * ACTION_NUM + a] = (1 / MU) * (v + BETA_T * V);
                scale = std::max(nume[2 * ACTION_NUM + a] - MAX_SCALE, scale);
            }
            for (a = 0; a < 3 * ACTION_NUM; a++) {
                nume[a] -= scale;
                deno += std::exp(nume[a]);
            }

            // 累積確率
            std::vector<double> accumProb(ACTION_NUM * 3 + 1, 0);
            for (a = 0; a < 3 * ACTION_NUM; a++) {
                prob = std::exp(nume[a]) / deno;
                // std::cout << a << ": " << prob << std::endl;
                accumProb[a + 1] = accumProb[a] + prob;
            }
            // std::cout << accumProb[13] << std::endl;

            // 乱数で行動を選択
            rn = rand(mt);
            auto iter = std::upper_bound(accumProb.begin(), accumProb.end(), rn);
            selected = iter - accumProb.begin() - 1;
            // std::cout << selected << std::endl;

            // 収入の計算・状態を更新
            if (selected >= 2 * ACTION_NUM) {
                continue;
            }
            revenueAvg += ((selected < ACTION_NUM) ? P[selected] - std::min(P[selected], s) : P[selected - ACTION_NUM]) / MONTE_NUM;
            s = ((selected < ACTION_NUM) ? s - std::min(P[selected], s) + R * P[selected] : s + std::min(R * P[selected - ACTION_NUM], MAX_POINT - s));
        }

        if (i % 100000 == 0) {
            std::cout << i << "回目終了" << std::endl;
            std::cout << "Average Revenue = " << revenueAvg << std::endl;
        }
    }

    return revenueAvg;
}