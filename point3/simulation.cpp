#include "simulation.hpp"


// The Proportional-Switch Adjustment Process
double FlowDynamics(std::vector<Path> &pathVec, std::vector<Demand> demandVec, std::vector<std::vector<double>> &perturbationTrans)
{
    int i, j;
    double V, diff;
    std::vector<Path> prev = pathVec;
    std::vector<double> perturbation;

    if (PATH_MODE) {
        perturbation = std::vector<double>(pathVec.size());
    }

    diff = 0;
    for (auto &&demand : demandVec) {
        for (auto &&idx1 : demand.pathIdx) {
            V = 0;
            for (auto &&idx2 : demand.pathIdx) {
                V += DYNAMICS_STEP * (prev[idx2].f * std::max(prev[idx2].pathCostWithPrice - pathVec[idx1].pathCostWithPrice, 0.0) - pathVec[idx1].f * std::max(pathVec[idx1].pathCostWithPrice - prev[idx2].pathCostWithPrice, 0.0));
            }
            diff += std::fabs(V);
            pathVec[idx1].f = pathVec[idx1].f + V;
            if (PATH_MODE) {
                perturbation[idx1] = V;
            }
        }
    }

    if (PATH_MODE) {
        perturbationTrans.emplace_back(perturbation);
    }

    return diff;
}


// 保有ポイント数のダイナミクス
void PointDynamics(std::vector<Path> pathVec, std::vector<Link> linkVec, std::vector<Demand> &demandVec)
{
    double V;

    for (auto &&demand : demandVec) {
        V = 0;
        for (auto &&pathIdx : demand.pathIdx) {
            for (auto &&linkIdx : pathVec[pathIdx].linkSeries) {
                V += pathVec[pathIdx].f * linkVec[linkIdx].point;
                // std::cout << linkVec[linkIdx].point << " ";
            }
        }
        V -= demand.L;
        // std::cout << demand.L << " ";
        V /= demand.m;
        demand.B += V;
        // std::cout << V << " ";
    }
    // std::cout << std::endl;
}


// ランダムな初期解の生成（端点）
void MakeRandomInitFlow(std::vector<Path> &pathVec, std::vector<Demand> demandVec)
{
    std::random_device rnd;
    std::mt19937 mt(rnd());
    std::uniform_real_distribution<> rand(0, 1);
    double sum, rnum, minval;
    int argminval;

    // 乱数で初期化
    for (auto &&demand : demandVec) {
        std::map<int, double> ratio;
        sum = 0;
        minval = 2;
        argminval = 1 << 29;
        for (auto &&idx : demand.pathIdx) {
            if (std::find(POI_01.begin(), POI_01.end(), idx) == POI_01.end() && std::find(POI_02.begin(), POI_02.end(), idx) == POI_02.end()) {
                rnum = 0;
            } else {
                rnum = rand(mt);
            }

            if (minval > rnum) {
                minval = rnum;
                argminval = idx;
            }

            ratio.insert({idx, rnum});
            sum += rnum;
        }

        sum -= minval;
        for (auto &&idx : demand.pathIdx) {
            if (idx == argminval) {
                pathVec[idx].f = 0;
                continue;
            }
            pathVec[idx].f = demand.m * ratio[idx] / sum;
        }
    }
}


// 初期解の生成
void MakeInitFlow(std::vector<Path> &pathVec, std::vector<Link> &linkVec, std::vector<Demand> demandVec, std::function<double(double, double)> RailLinkCostFunc, std::function<double(double, double)> StayLinkCostFunc, std::function<double(double, double)> StayLinkCostFunc2)
{
    double diffPrev, diff, diffDiff;
    int convergeCount, count, idx;
    std::vector<std::vector<double>> tmpTrans;

    // 適当に初期化
    for (auto &&demand : demandVec) {
        for (auto &&idx : demand.pathIdx) {
            pathVec[idx].f = demand.m / static_cast<double>(demand.pathIdx.size());
            // std::cout << pathVec[idx].f << " ";
        }
    }

    convergeCount = 0;
    count = 0;
    diff = 1e9;
    diffPrev = 0;
    while (true) {
        // リンクベース変数の計算
        for (auto &&link : linkVec) {
            link.CalcLinkFlow(pathVec);  // リンクフローの計算
            link.CalcRailLinkCost(RailLinkCostFunc, StationLinkCostFunc, linkVec);  // 鉄道リンクのリンクコスト計算
        }

        // パスベースの変数の計算
        for (auto &&path : pathVec) {
            path.CalcPathCost(linkVec, demandVec, StayLinkCostFunc, StayLinkCostFunc2);
            path.CalcPathCostWithPrice(linkVec);  // 一緒だけど
        }

        // 収束判定
        diffDiff = std::fabs(diff - diffPrev);
        if (diffDiff < CONVERGE_THRES) {
            convergeCount++;
            if (convergeCount >= CONVERGE_COUNT_THRES) {
                break;
            }
        } else {
            convergeCount = 0;
        }

        // ダイナミクスに従って翌日のフローの確定
        diffPrev = diff;
        diff = FlowDynamics(pathVec, demandVec, tmpTrans);

        count++;
        if (count % 1 == 0) {
            std::cout << count << "回目終了 | diffDiff = " << diffDiff << std::endl;
        }
    }
}


// 金銭での課金額とポイント付与額決定
void CalcMoneyPoint(std::vector<Path> pathVec, std::vector<Link> &linkVec, std::vector<Node> nodeVec, std::vector<Demand> demandVec)
{
    bool isOK;
    double getPoints;

    for (auto &&link : linkVec) {
        if (link.type == 'r' || link.type == 'i') {
            link.money = link.price;
        }
    }

    while (true) {
        // 解計算
        for (auto &&link : linkVec) {
            if (link.type == 'r' || link.type == 'i') {
                link.point = (link.money - link.price) / GAMMA;
            }
        }

        // 判定
        isOK = true;
        for (auto &&demand : demandVec) {
            if (demand.B - demand.L / demand.m >= M_MAX) {
                continue;
            }
            getPoints = 0;
            for (auto pathIdx : demand.pathIdx) {
                for (auto &&linkIdx : pathVec[pathIdx].linkSeries) {
                    if (linkVec[linkIdx].type == 'r' || linkVec[linkIdx].type == 'i') {
                        getPoints += pathVec[pathIdx].f * linkVec[linkIdx].point;
                        // std::cout << pathIdx << ": " << pathVec[pathIdx].f << " | ";
                    }
                }
            }
            
            if (getPoints <= demand.L) {
                isOK = false;
                // std::cout << getPoints << " ";
                break;
            }
        }

        // 条件を満たしていれば計算終了
        if (isOK) {
            break;
        }

        // 金銭での課金額更新
        for (auto &&link : linkVec) {
            if (link.type == 'r' || link.type == 'i') {
                link.money += DELTA_P;
            }
        }
    }
}


// With Pointのシミュレーション
void Simulation(std::vector<Path> &pathVec, std::vector<Link> &linkVec, std::vector<Node> nodeVec, std::vector<Demand> &demandVec, std::function<double(double, double)> RailLinkCostFunc, std::function<double(double, double)> RailLinkCostFuncPrime, std::function<double(double, double)> StayLinkCostFunc, std::function<double(double, double)> StayLinkCostFunc2, std::vector<double> &sumCostTrans, std::vector<double> &railCostTrans, std::vector<double> &stayUtilTrans, std::vector<std::vector<double>> &pathCostTrans, std::vector<std::vector<double>> &possessedPointTrans, std::vector<std::vector<double>> &linkFlowTrans, std::vector<std::vector<double>> &zeroFlowTrans, std::vector<std::vector<double>> &oneFlowTrans, std::vector<std::vector<double>> &pathFlowTrans, std::vector<std::vector<double>> &moneyTrans, std::vector<std::vector<double>> &pointTrans, std::vector<double> &railRevenueTrans, std::vector<double> &storeRevenueTrans, std::vector<std::vector<double>> &perturbationTrans, std::vector<std::vector<double>> &pathZeroCostTrans, std::vector<std::vector<double>> &pathOneCostTrans)
{
    double diff, sumCost, railCost, stayUtil, railRevenue, storeRevenue;
    int convergeCount, count, i;
    std::vector<double> pathCost, possessedPoint(demandVec.size()), linkFlow(linkVec.size()), linkMoney(linkVec.size()), linkPoint(linkVec.size()), zeroFlow(linkVec.size()), oneFlow(linkVec.size()), pathFlow, pathZeroCost(4), pathOneCost(4);

    if (PATH_MODE) {
        pathCost = std::vector<double>(pathVec.size());
        pathFlow = std::vector<double>(pathVec.size());
    }

    convergeCount = 0;
    count = 0;
    while (true) {
        // リンクベースの変数の計算
        railCost = 0;
        for (auto &&link : linkVec) {
            link.CalcLinkFlow(pathVec);
            link.CalcRailLinkCost(RailLinkCostFunc, StationLinkCostFunc, linkVec);
            link.CalcRailLinkCostPrime(RailLinkCostFuncPrime, StationLinkCostFuncPrime, linkVec);
            link.CalcLinkPrice();
            if (link.type == 'r') {
                railCost += link.linkCost * link.x;
            }
        }
        // std::cout << "リンクベースの変数の計算完了" << std::endl;

        // パスベースの変数の計算
        sumCost = 0;
        stayUtil = 0;
        for (auto &&path : pathVec) {
            path.CalcPathCost(linkVec, demandVec, StayLinkCostFunc, StayLinkCostFunc2);
            path.CalcPathCostWithPrice(linkVec);
            // std::cout << path.f << " ";
            sumCost += path.f * path.pathCost;

            for (auto &&idx : path.linkSeries) {
                if (linkVec[idx].type == 's') {
                    stayUtil -= StayLinkCostFunc(demandVec[path.odidx].B, linkVec[idx].poiU) * path.f;
                    stayUtil -= BETA_H * DELTA_T * path.f;
                }
                if (linkVec[idx].type == 't') {
                    stayUtil -= StayLinkCostFunc2(demandVec[path.odidx].B, linkVec[idx].poiU) * path.f;
                    stayUtil -= BETA_H * DELTA_T * path.f;
                }
            }
        }
        // std::cout << "パスベースの変数の計算完了" << std::endl;

        // 経路ごとの混雑コスト・滞在コストの計算
        pathZeroCost = std::vector<double>(4, 0);
        pathOneCost = std::vector<double>(4, 0);
        for (auto &&idx : pathVec[1].linkSeries) {
            if (linkVec[idx].type == 'r') {
                pathZeroCost[0] += linkVec[idx].linkCost; // * pathVec[0].f;
                pathZeroCost[3] += linkVec[idx].price;
            }
            if (linkVec[idx].type == 's') {
                pathZeroCost[1] += StayLinkCostFunc(demandVec[pathVec[1].odidx].B, linkVec[idx].poiU); // * pathVec[0].f;
                pathZeroCost[1] += BETA_H * DELTA_T;
            }
            if (linkVec[idx].type == 't') {
                pathZeroCost[1] += StayLinkCostFunc2(demandVec[pathVec[1].odidx].B, linkVec[idx].poiU); // * pathVec[0].f;
                pathZeroCost[1] += BETA_H * DELTA_T;
            }
        }
        pathZeroCost[2] = pathVec[1].pathCost; // * pathVec[0].f;

        for (auto &&idx : pathVec[2].linkSeries) {
            if (linkVec[idx].type == 'r') {
                pathOneCost[0] += linkVec[idx].linkCost; // * pathVec[1].f;
                pathOneCost[3] += linkVec[idx].price;
            }
            if (linkVec[idx].type == 's') {
                pathOneCost[1] += StayLinkCostFunc(demandVec[pathVec[2].odidx].B, linkVec[idx].poiU); // * pathVec[1].f;
                pathOneCost[1] += BETA_H * DELTA_T;
            }
            if (linkVec[idx].type == 't') {
                pathOneCost[1] += StayLinkCostFunc2(demandVec[pathVec[1].odidx].B, linkVec[idx].poiU); // * pathVec[1].f;
                pathOneCost[1] += BETA_H * DELTA_T;
            }
        }
        pathOneCost[2] = pathVec[2].pathCost; // * pathVec[1].f;

        // 使用ポイント数・駅まちでの鉄道会社の収入計算
        storeRevenue = 0;
        for (auto &&demand : demandVec) {
            storeRevenue += demand.CalcConsumedPoint(pathVec, linkVec);
        }
        // std::cout << "課金額・ポイント付与額の計算の手前まで終了" << std::endl;

        // 金銭での課金額とポイント付与額計算
        CalcMoneyPoint(pathVec, linkVec, nodeVec, demandVec);
        // std::cout << "課金額・ポイント付与額計算完了" << std::endl;

        // 記録
        for (i = 0; i < static_cast<int>(demandVec.size()); i++) {
            possessedPoint[i] = demandVec[i].B;
        }
        railRevenue = 0;
        for (i = 0; i < static_cast<int>(linkVec.size()); i++) {
            linkFlow[i] = linkVec[i].x;
            zeroFlow[i] = linkVec[i].xFromZero;
            oneFlow[i] = linkVec[i].xFromOne;
            linkMoney[i] = linkVec[i].money;
            linkPoint[i] = linkVec[i].point;
            if (linkVec[i].type == 'r') {
                railRevenue += linkVec[i].x * linkVec[i].money;
            }
        }

        sumCostTrans.emplace_back(sumCost);
        railCostTrans.emplace_back(railCost);
        stayUtilTrans.emplace_back(stayUtil);
        possessedPointTrans.emplace_back(possessedPoint);
        linkFlowTrans.emplace_back(linkFlow);
        zeroFlowTrans.emplace_back(zeroFlow);
        oneFlowTrans.emplace_back(oneFlow);
        moneyTrans.emplace_back(linkMoney);
        pointTrans.emplace_back(linkPoint);
        railRevenueTrans.emplace_back(railRevenue);
        storeRevenueTrans.emplace_back(storeRevenue);
        pathZeroCostTrans.emplace_back(pathZeroCost);
        pathOneCostTrans.emplace_back(pathOneCost);
        if (PATH_MODE) {
            for (i = 0; i < static_cast<int>(pathVec.size()); i++) {
                pathCost[i] = pathVec[i].pathCostWithPrice;
                pathFlow[i] = pathVec[i].f;
                // std::cout << pathFlow[i] << " ";
            }
            pathCostTrans.emplace_back(pathCost);
            pathFlowTrans.emplace_back(pathFlow);
        }

        // 収束判定
        // if (diff < CONVERGE_THRES) {
        //     convergeCount++;
        //     if (convergeCount >= CONVERGE_COUNT_THRES) {
        //         break;
        //     }
        // } else {
        //     convergeCount = 0;
        // }
        if (count >= MAX_ITER) {
            break;
        }
        if (count % 1 == 0) {
            std::cout << count << "日目終了" << std::endl;
        }

        // ポイント数の変化
        PointDynamics(pathVec, linkVec, demandVec);

        // ダイナミクスに従って翌日のフロー決定
        diff = FlowDynamics(pathVec, demandVec, perturbationTrans);

        count++;
    }
}


// Without Pointのシミュレーション
void SimulationWithout(std::vector<Path> &pathVec, std::vector<Link> &linkVec, std::vector<Demand> &demandVec, std::function<double(double, double)> RailLinkCostFunc, std::function<double(double, double)> RailLinkCostFuncPrime, std::function<double(double, double)> StayLinkCostFunc, std::function<double(double, double)> StayLinkCostFunc2, std::vector<double> &sumCostTrans, std::vector<double> &railCostTrans, std::vector<double> &stayUtilTrans, std::vector<std::vector<double>> &pathCostTrans, std::vector<std::vector<double>> &possessedPointTrans, std::vector<std::vector<double>> &linkFlowTrans, std::vector<std::vector<double>> &zeroFlowTrans, std::vector<std::vector<double>> &oneFlowTrans, std::vector<std::vector<double>> &pathFlowTrans, std::vector<std::vector<double>> &moneyTrans, std::vector<std::vector<double>> &pointTrans, std::vector<double> &railRevenueTrans, std::vector<double> &storeRevenueTrans, std::vector<std::vector<double>> &perturbationTrans, std::vector<std::vector<double>> &pathZeroCostTrans, std::vector<std::vector<double>> &pathOneCostTrans)
{
    double diff, sumCost, railCost, stayUtil, railRevenue, storeRevenue;
    int convergeCount, count, i;
    std::vector<double> pathCost, possessedPoint(demandVec.size()), linkFlow(linkVec.size()), linkMoney(linkVec.size()), linkPoint(linkVec.size()), zeroFlow(linkVec.size()), oneFlow(linkVec.size()), pathFlow, pathZeroCost(4), pathOneCost(4);
    if (PATH_MODE) {
        pathCost = std::vector<double>(pathVec.size());
        pathFlow = std::vector<double>(pathVec.size());
    }

    convergeCount = 0;
    count = 0;
    while (true) {
        // リンクベースの変数の計算
        railCost = 0;
        for (auto &&link : linkVec) {
            link.CalcLinkFlow(pathVec);
            link.CalcRailLinkCost(RailLinkCostFunc, StationLinkCostFunc, linkVec);
            link.CalcRailLinkCostPrime(RailLinkCostFuncPrime, StationLinkCostFuncPrime, linkVec);
            link.CalcLinkPrice();
            link.money = link.price;
            link.point = 0;
            if (link.type == 'r') {
                railCost += link.linkCost * link.x;
            }
        }

        // パスベースの変数の計算
        sumCost = 0;
        stayUtil = 0;
        for (auto &&path : pathVec) {
            path.CalcPathCost(linkVec, demandVec, StayLinkCostFunc, StayLinkCostFunc2);
            path.CalcPathCostWithPrice(linkVec);
            sumCost += path.f * path.pathCost;

            for (auto &&idx : path.linkSeries) {
                if (linkVec[idx].type == 's') {
                    stayUtil -= StayLinkCostFunc(demandVec[path.odidx].B, linkVec[idx].poiU) * path.f;
                    stayUtil -= BETA_H * DELTA_T * path.f;
                }
                if (linkVec[idx].type == 't') {
                    stayUtil -= StayLinkCostFunc2(demandVec[path.odidx].B, linkVec[idx].poiU) * path.f;
                    stayUtil -= BETA_H * DELTA_T * path.f;
                }
            }
        }

        // 経路ごとの混雑コスト・滞在コストの計算
        pathZeroCost = std::vector<double>(4, 0);
        pathOneCost = std::vector<double>(4, 0);
        for (auto &&idx : pathVec[1].linkSeries) {
            if (linkVec[idx].type == 'r') {
                pathZeroCost[0] += linkVec[idx].linkCost; // * pathVec[0].f;
                pathZeroCost[3] += linkVec[idx].price;
            }
            if (linkVec[idx].type == 's') {
                pathZeroCost[1] += StayLinkCostFunc(demandVec[pathVec[1].odidx].B, linkVec[idx].poiU); // * pathVec[0].f;
                pathZeroCost[1] += BETA_H * DELTA_T;
            }
            if (linkVec[idx].type == 't') {
                pathZeroCost[1] += StayLinkCostFunc2(demandVec[pathVec[1].odidx].B, linkVec[idx].poiU); // * pathVec[0].f;
                pathZeroCost[1] += BETA_H * DELTA_T;
            }
        }
        pathZeroCost[2] = pathVec[1].pathCost; // * pathVec[0].f;

        for (auto &&idx : pathVec[2].linkSeries) {
            if (linkVec[idx].type == 'r') {
                pathOneCost[0] += linkVec[idx].linkCost; // * pathVec[1].f;
                pathOneCost[3] += linkVec[idx].price;
            }
            if (linkVec[idx].type == 's') {
                pathOneCost[1] += StayLinkCostFunc(demandVec[pathVec[2].odidx].B, linkVec[idx].poiU); // * pathVec[1].f;
                pathOneCost[1] += BETA_H * DELTA_T;
            }
            if (linkVec[idx].type == 't') {
                pathOneCost[1] += StayLinkCostFunc2(demandVec[pathVec[1].odidx].B, linkVec[idx].poiU); // * pathVec[1].f;
                pathOneCost[1] += BETA_H * DELTA_T;
            }
        }
        pathOneCost[2] = pathVec[2].pathCost; // * pathVec[1].f;

        // 使用ポイント数・駅まちでの鉄道会社の収入計算
        storeRevenue = 0;
        for (auto &&demand : demandVec) {
            storeRevenue += demand.CalcConsumedPoint(pathVec, linkVec);
        }

        // 記録
        for (i = 0; i < static_cast<int>(demandVec.size()); i++) {
            possessedPoint[i] = demandVec[i].B;
        }
        railRevenue = 0;
        for (i = 0; i < static_cast<int>(linkVec.size()); i++) {
            linkFlow[i] = linkVec[i].x;
            zeroFlow[i] = linkVec[i].xFromZero;
            oneFlow[i] = linkVec[i].xFromOne;
            linkMoney[i] = linkVec[i].money;
            linkPoint[i] = linkVec[i].point;
            if (linkVec[i].type == 'r') {
                railRevenue += linkVec[i].x * linkVec[i].money;
            }
        }

        sumCostTrans.emplace_back(sumCost);
        railCostTrans.emplace_back(railCost);
        stayUtilTrans.emplace_back(stayUtil);
        possessedPointTrans.emplace_back(possessedPoint);
        linkFlowTrans.emplace_back(linkFlow);
        zeroFlowTrans.emplace_back(zeroFlow);
        oneFlowTrans.emplace_back(oneFlow);
        moneyTrans.emplace_back(linkMoney);
        pointTrans.emplace_back(linkPoint);
        railRevenueTrans.emplace_back(railRevenue);
        storeRevenueTrans.emplace_back(storeRevenue);
        pathZeroCostTrans.emplace_back(pathZeroCost);
        pathOneCostTrans.emplace_back(pathOneCost);
        if (PATH_MODE) {
            for (i = 0; i < static_cast<int>(pathVec.size()); i++) {
                pathCost[i] = pathVec[i].pathCostWithPrice;
                pathFlow[i] = pathVec[i].f;
            }
            pathCostTrans.emplace_back(pathCost);
            pathFlowTrans.emplace_back(pathFlow);
        }

        // 収束判定
        // if (diff < CONVERGE_THRES) {
        //     convergeCount++;
        //     if (convergeCount >= CONVERGE_COUNT_THRES) {
        //         break;
        //     }
        // } else {
        //     convergeCount = 0;
        // }
        if (count >= MAX_ITER) {
            break;
        }
        if (count % 10 == 0) {
            std::cout << count << "日目終了" << std::endl;
        }

        // ポイント数の変化
        PointDynamics(pathVec, linkVec, demandVec);

        // ダイナミクスに従って翌日のフロー決定
        diff = FlowDynamics(pathVec, demandVec, perturbationTrans);

        count++;
    }
}


// Without Controlのシミュレーション
void SimulationNoControl(std::vector<Path> &pathVec, std::vector<Link> &linkVec, std::vector<Node> nodeVec, std::vector<Demand> &demandVec, std::function<double(double, double)> RailLinkCostFunc, std::function<double(double, double)> RailLinkCostFuncPrime, std::function<double(double, double)> StayLinkCostFunc, std::function<double(double, double)> StayLinkCostFunc2, std::vector<double> &sumCostTrans, std::vector<double> &railCostTrans, std::vector<double> &stayUtilTrans, std::vector<std::vector<double>> &pathCostTrans, std::vector<std::vector<double>> &possessedPointTrans, std::vector<std::vector<double>> &linkFlowTrans, std::vector<std::vector<double>> &zeroFlowTrans, std::vector<std::vector<double>> &oneFlowTrans, std::vector<std::vector<double>> &pathFlowTrans, std::vector<std::vector<double>> &moneyTrans, std::vector<std::vector<double>> &pointTrans, std::vector<double> &railRevenueTrans, std::vector<double> &storeRevenueTrans, std::vector<std::vector<double>> &perturbationTrans, std::vector<std::vector<double>> &pathZeroCostTrans, std::vector<std::vector<double>> &pathOneCostTrans)
{
    double diff, sumCost, railCost, stayUtil, railRevenue, storeRevenue;
    int convergeCount, count, i;
    std::vector<double> pathCost, possessedPoint(demandVec.size()), linkFlow(linkVec.size()), linkMoney(linkVec.size()), linkPoint(linkVec.size()), zeroFlow(linkVec.size()), oneFlow(linkVec.size()), pathFlow, pathZeroCost(4), pathOneCost(4);
    if (PATH_MODE) {
        pathCost = std::vector<double>(pathVec.size());
        pathFlow = std::vector<double>(pathVec.size());
    }

    convergeCount = 0;
    count = 0;
    while (true) {
        // リンクベースの変数の計算
        railCost = 0;
        for (auto &&link : linkVec) {
            link.CalcLinkFlow(pathVec);
            link.CalcRailLinkCost(RailLinkCostFunc, StationLinkCostFunc, linkVec);
            link.CalcRailLinkCostPrime(RailLinkCostFuncPrime, StationLinkCostFuncPrime, linkVec);
            if (link.type == 'r') {
                railCost += link.linkCost * link.x;
            }
        }

        // パスベースの変数の計算
        sumCost = 0;
        stayUtil = 0;
        for (auto &&path : pathVec) {
            path.CalcPathCost(linkVec, demandVec, StayLinkCostFunc, StayLinkCostFunc2);
            path.CalcPathCostWithPrice(linkVec);
            sumCost += path.f * path.pathCost;

            for (auto &&idx : path.linkSeries) {
                if (linkVec[idx].type == 's') {
                    stayUtil -= StayLinkCostFunc(demandVec[path.odidx].B, linkVec[idx].poiU) * path.f;
                    stayUtil -= BETA_H * DELTA_T * path.f;
                }
                if (linkVec[idx].type == 't') {
                    stayUtil -= StayLinkCostFunc2(demandVec[path.odidx].B, linkVec[idx].poiU) * path.f;
                    stayUtil -= BETA_H * DELTA_T * path.f;
                }
            }
        }

        // 経路ごとの混雑コスト・滞在コストの計算
        pathZeroCost = std::vector<double>(4, 0);
        pathOneCost = std::vector<double>(4, 0);
        for (auto &&idx : pathVec[1].linkSeries) {
            if (linkVec[idx].type == 'r') {
                pathZeroCost[0] += linkVec[idx].linkCost; // * pathVec[0].f;
                pathZeroCost[3] += linkVec[idx].price;
            }
            if (linkVec[idx].type == 's') {
                pathZeroCost[1] += StayLinkCostFunc(demandVec[pathVec[1].odidx].B, linkVec[idx].poiU); // * pathVec[0].f;
                pathZeroCost[1] += BETA_H * DELTA_T;
            }
            if (linkVec[idx].type == 't') {
                pathZeroCost[1] += StayLinkCostFunc2(demandVec[pathVec[1].odidx].B, linkVec[idx].poiU); // * pathVec[0].f;
                pathZeroCost[1] += BETA_H * DELTA_T;
            }
        }
        pathZeroCost[2] = pathVec[1].pathCost; // * pathVec[0].f;

        for (auto &&idx : pathVec[2].linkSeries) {
            if (linkVec[idx].type == 'r') {
                pathOneCost[0] += linkVec[idx].linkCost; // * pathVec[1].f;
                pathOneCost[3] += linkVec[idx].price;
            }
            if (linkVec[idx].type == 's') {
                pathOneCost[1] += StayLinkCostFunc(demandVec[pathVec[2].odidx].B, linkVec[idx].poiU); // * pathVec[1].f;
                pathOneCost[1] += BETA_H * DELTA_T;
            }
            if (linkVec[idx].type == 't') {
                pathOneCost[1] += StayLinkCostFunc(demandVec[pathVec[1].odidx].B, linkVec[idx].poiU); // * pathVec[1].f;
                pathOneCost[1] += BETA_H * DELTA_T;
            }
        }
        pathOneCost[2] = pathVec[2].pathCost; // * pathVec[1].f;

        // 使用ポイント数・駅まちでの鉄道会社の収入計算
        storeRevenue = 0;
        for (auto &&demand : demandVec) {
            storeRevenue += demand.CalcConsumedPoint(pathVec, linkVec);
        }

        // 記録
        for (i = 0; i < static_cast<int>(demandVec.size()); i++) {
            possessedPoint[i] = demandVec[i].B;
        }
        railRevenue = 0;
        for (i = 0; i < static_cast<int>(linkVec.size()); i++) {
            linkFlow[i] = linkVec[i].x;
            zeroFlow[i] = linkVec[i].xFromZero;
            oneFlow[i] = linkVec[i].xFromOne;
            linkMoney[i] = linkVec[i].money;
            linkPoint[i] = linkVec[i].point;
            if (linkVec[i].type == 'r') {
                railRevenue += linkVec[i].x * linkVec[i].money;
            }
        }
        sumCostTrans.emplace_back(sumCost);
        railCostTrans.emplace_back(railCost);
        stayUtilTrans.emplace_back(stayUtil);
        possessedPointTrans.emplace_back(possessedPoint);
        linkFlowTrans.emplace_back(linkFlow);
        zeroFlowTrans.emplace_back(zeroFlow);
        oneFlowTrans.emplace_back(oneFlow);
        moneyTrans.emplace_back(linkMoney);
        pointTrans.emplace_back(linkPoint);
        railRevenueTrans.emplace_back(railRevenue);
        storeRevenueTrans.emplace_back(storeRevenue);
        pathZeroCostTrans.emplace_back(pathZeroCost);
        pathOneCostTrans.emplace_back(pathOneCost);
        if (PATH_MODE) {
            for (i = 0; i < static_cast<int>(pathVec.size()); i++) {
                pathCost[i] = pathVec[i].pathCostWithPrice;
                pathFlow[i] = pathVec[i].f;
            }
            pathCostTrans.emplace_back(pathCost);
            pathFlowTrans.emplace_back(pathFlow);
        }

        if (count >= MAX_ITER) {
            break;
        }
        if (count % 10 == 0) {
            std::cout << count << "日目終了" << std::endl;
        }

        // ダイナミクスに従って翌日のフロー決定
        diff = FlowDynamics(pathVec, demandVec, perturbationTrans);

        count++;
    }
}