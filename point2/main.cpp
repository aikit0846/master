#include "global.hpp"
#include "input.hpp"
#include "simulation.hpp"
#include "output.hpp"

////// TODO
// 変数の整理: done
// Pythonファイルを整理する: done
// MODEとSUBDIRはコマンドライン変数とする: done
// 制御なしの相図を出す: done


int main(int argc, char *argv[])
{
    // コマンドライン引数の受け取り
    const int MODE = argv[1][0] - '0';
    const std::string SUBDIR = argv[2];

    // 時間測定開始
    std::chrono::system_clock::time_point start, middle, end;
    double elapsed;
    start = std::chrono::system_clock::now();
    std::cout << "プログラム開始" << std::endl;

    // ノードデータ読み込み
    std::vector<Node> nodeVec = ReadNodeFile(DIR + SUBDIR + NODE_FNAME);

    // リンクデータ読み込み
    std::vector<Link> linkVec = ReadLinkFile(DIR + SUBDIR + LINK_FNAME);

    // 需要データ読み込み
    std::vector<Demand> demandVec = ReadDemandFile(DIR + SUBDIR + DEMAND_FNAME);

    // 経路データ読み込み，リンクデータ・需要データとの紐付け
    std::vector<Path> pathVec = ReadPathFile(DIR + SUBDIR + PATH_FNAME, linkVec, demandVec);
    std::cout << "データ読み込み完了" << std::endl;

    // 記録用vectorの準備
    std::vector<double> sumCostTrans, railCostTrans, stayUtilTrans, railRevenueTrans, storeRevenueTrans;
    std::vector<std::vector<double>> possessedPointTrans, linkFlowTrans, moneyTrans, pointTrans, zeroFlowTrans, oneFlowTrans, pathCostTrans, pathFlowTrans, perturbationTrans, pathZeroCostTrans, pathOneCostTrans;
    std::vector<double> sumCostTransOut, railCostTransOut, stayUtilTransOut, railRevenueTransOut, storeRevenueTransOut;
    std::vector<std::vector<double>> possessedPointTransOut, linkFlowTransOut, moneyTransOut, pointTransOut, zeroFlowTransOut, oneFlowTransOut, pathCostTransOut, pathFlowTransOut, perturbationTransOut, pathZeroCostTransOut, pathOneCostTransOut;
    std::vector<double> sumCostTransNo, railCostTransNo, stayUtilTransNo, railRevenueTransNo, storeRevenueTransNo;
    std::vector<std::vector<double>> possessedPointTransNo, linkFlowTransNo, moneyTransNo, pointTransNo, zeroFlowTransNo, oneFlowTransNo, pathCostTransNo, pathFlowTransNo, perturbationTransNo, pathZeroCostTransNo, pathOneCostTransNo;


    // 初期解記録用
    std::vector<Path> initPathVec, initPathVec2;
    std::vector<Link> initLinkVec, initLinkVec2;

    if (MODE == 0) {
        // 均衡解出発のシミュレーション
        std::cout << "均衡解スタートのシミュレーション開始" << std::endl;

        // With Pointでシミュレーション
        std::cout << "初期解作成開始" << std::endl;
        MakeInitFlow(pathVec, linkVec, demandVec, RailLinkCostFunc, StayLinkCostFunc, StayLinkCostFunc2);
        initPathVec = pathVec;
        initLinkVec = linkVec;
        initPathVec2 = pathVec;
        initLinkVec2 = linkVec;
        std::cout << "初期解作成完了" << std::endl;
        std::cout << "With Pointシミュレーション開始" << std::endl;
        Simulation(pathVec, linkVec, nodeVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTrans, railCostTrans, stayUtilTrans, pathCostTrans, possessedPointTrans, linkFlowTrans, zeroFlowTrans, oneFlowTrans, pathFlowTrans, moneyTrans, pointTrans, railRevenueTrans, storeRevenueTrans, perturbationTrans, pathZeroCostTrans, pathOneCostTrans);
        std::cout << "With Pointシミュレーション終了" << std::endl;

        middle = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(middle - start).count();
        std::cout << elapsed << "秒経過" << std::endl;

        // Without Pointでシミュレーション
        for (auto &&demand : demandVec) {
            demand.B = 0;
            demand.L = 0;
        }
        std::cout << "Without Pointシミュレーション開始" << std::endl;
        SimulationWithout(initPathVec, initLinkVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTransOut, railCostTransOut, stayUtilTransOut, pathCostTransOut, possessedPointTransOut, linkFlowTransOut, zeroFlowTransOut, oneFlowTransOut, pathFlowTransOut, moneyTransOut, pointTransOut, railRevenueTransOut, storeRevenueTransOut, perturbationTransOut, pathZeroCostTransOut, pathOneCostTransOut);
        std::cout << "Without Pointシミュレーション終了" << std::endl;

        middle = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(middle - start).count();
        std::cout << elapsed << "秒経過" << std::endl;

        // Without Controlでシミュレーション
        for (auto &&demand : demandVec) {
            demand.B = 0;
            demand.L = 0;
        }
        std::cout << "Without Controlシミュレーション開始" << std::endl;
        SimulationNoControl(initPathVec2, initLinkVec2, nodeVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTransNo, railCostTransNo, stayUtilTransNo, pathCostTransNo, possessedPointTransNo, linkFlowTransNo, zeroFlowTransNo, oneFlowTransNo, pathFlowTransNo, moneyTransNo, pointTransNo, railRevenueTransNo, storeRevenueTransNo, perturbationTransNo, pathZeroCostTransNo, pathOneCostTransNo);     
        std::cout << "Without Controlシミュレーション終了" << std::endl;

        middle = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(middle - start).count();
        std::cout << elapsed << "秒経過" << std::endl;

        // CSV出力
        std::cout << "ファイル書き込み開始" << std::endl;
        Write1DFile(sumCostTrans, DIR + SUBDIR + SUMCOST_FNAME, "sumCost");
        Write1DFile(railCostTrans, DIR + SUBDIR + RAILCOST_FNAME, "railCost");
        Write1DFile(stayUtilTrans, DIR + SUBDIR + STAYUTIL_FNAME, "stayUtil");
        Write2DFile(pathCostTrans, DIR + SUBDIR + PATHCOST_FNAME);
        Write2DFile(possessedPointTrans, DIR + SUBDIR + POSSESSED_FNAME);
        Write2DFile(linkFlowTrans, DIR + SUBDIR + LINKFLOW_FNAME);
        Write2DFile(zeroFlowTrans, DIR + SUBDIR + ZEROFLOW_FNAME);
        Write2DFile(oneFlowTrans, DIR + SUBDIR + ONEFLOW_FNAME);
        Write2DFile(pathFlowTrans, DIR + SUBDIR + PATHFLOW_FNAME);
        Write2DFile(moneyTrans, DIR + SUBDIR + MONEY_FNAME);
        Write2DFile(pointTrans, DIR + SUBDIR + POINT_FNAME);
        Write1DFile(railRevenueTrans, DIR + SUBDIR + RAILREV_FNAME, "railRevenue");
        Write1DFile(storeRevenueTrans, DIR + SUBDIR + STOREREV_FNAME, "storeRevenue");
        Write2DFile(pathZeroCostTrans, DIR + SUBDIR + ZEROCOST_FNAME);
        Write2DFile(pathOneCostTrans, DIR + SUBDIR + ONECOST_FNAME);
        Write2DFile(perturbationTrans, DIR + SUBDIR + PERTURB_FNAME);
        Write1DFile(sumCostTransOut, DIR + SUBDIR + "out" + SUMCOST_FNAME, "sumCost");
        Write1DFile(railCostTransOut, DIR + SUBDIR + "out" + RAILCOST_FNAME, "railCost");
        Write1DFile(stayUtilTransOut, DIR + SUBDIR + "out" + STAYUTIL_FNAME, "stayUtil");
        Write2DFile(pathCostTransOut, DIR + SUBDIR + "out" + PATHCOST_FNAME);
        Write2DFile(possessedPointTransOut, DIR + SUBDIR + "out" + POSSESSED_FNAME);
        Write2DFile(linkFlowTransOut, DIR + SUBDIR + "out" + LINKFLOW_FNAME);
        Write2DFile(zeroFlowTransOut, DIR + SUBDIR + "out" + ZEROFLOW_FNAME);
        Write2DFile(oneFlowTransOut, DIR + SUBDIR + "out" + ONEFLOW_FNAME);
        Write2DFile(pathFlowTransOut, DIR + SUBDIR + "out" + PATHFLOW_FNAME);
        Write2DFile(moneyTransOut, DIR + SUBDIR + "out" + MONEY_FNAME);
        Write2DFile(pointTransOut, DIR + SUBDIR + "out" + POINT_FNAME);
        Write1DFile(railRevenueTransOut, DIR + SUBDIR + "out" + RAILREV_FNAME, "railRevenue");
        Write1DFile(storeRevenueTransOut, DIR + SUBDIR + "out" + STOREREV_FNAME, "storeRevenue");
        Write2DFile(pathZeroCostTransOut, DIR + SUBDIR + "out" + ZEROCOST_FNAME);
        Write2DFile(pathOneCostTransOut, DIR + SUBDIR + "out" + ONECOST_FNAME);
        Write2DFile(perturbationTransOut, DIR + SUBDIR + "out" + PERTURB_FNAME);
        Write1DFile(sumCostTransNo, DIR + SUBDIR + "no" + SUMCOST_FNAME, "sumCost");
        Write1DFile(railCostTransNo, DIR + SUBDIR + "no" + RAILCOST_FNAME, "railCost");
        Write1DFile(stayUtilTransNo, DIR + SUBDIR + "no" + STAYUTIL_FNAME, "stayUtil");
        Write2DFile(pathCostTransNo, DIR + SUBDIR + "no" + PATHCOST_FNAME);
        Write2DFile(possessedPointTransNo, DIR + SUBDIR + "no" + POSSESSED_FNAME);
        Write2DFile(linkFlowTransNo, DIR + SUBDIR + "no" + LINKFLOW_FNAME);
        Write2DFile(zeroFlowTransNo, DIR + SUBDIR + "no" + ZEROFLOW_FNAME);
        Write2DFile(oneFlowTransNo, DIR + SUBDIR + "no" + ONEFLOW_FNAME);
        Write2DFile(pathFlowTransNo, DIR + SUBDIR + "no" + PATHFLOW_FNAME);
        Write2DFile(moneyTransNo, DIR + SUBDIR + "no" + MONEY_FNAME);
        Write2DFile(pointTransNo, DIR + SUBDIR + "no" + POINT_FNAME);
        Write1DFile(railRevenueTransNo, DIR + SUBDIR + "no" + RAILREV_FNAME, "railRevenue");
        Write1DFile(storeRevenueTransNo, DIR + SUBDIR + "no" + STOREREV_FNAME, "storeRevenue");
        Write2DFile(pathZeroCostTransNo, DIR + SUBDIR + "no" + ZEROCOST_FNAME);
        Write2DFile(pathOneCostTransNo, DIR + SUBDIR + "no" + ONECOST_FNAME);
        Write2DFile(perturbationTransNo, DIR + SUBDIR + "no" + PERTURB_FNAME);
        WriteParameterFile(SUBDIR);
        std::cout << "ファイル書き込み完了" << std::endl;
    } else if (MODE == 1) {
        // 等高線 & ベクトル場用
        std::cout << "等高線＆ベクトル場描画用のシミュレーション開始" << std::endl;

        // With Point
        for (double i = 0; i <= demandVec[0].m; i += 0.5) {
            for (double j = 0; j <= demandVec[0].m - i; j += 0.5) {
                for (auto &&demand : demandVec) {
                    demand.B = M_MAX;
                }
                std::vector<double> pathFlow(pathVec.size(), 0);

                pathVec[0].f = i;
                pathVec[1].f = j;
                pathVec[2].f = demandVec[0].m - i - j;

                for (auto &&link : linkVec) {
                    link.CalcLinkFlow(pathVec);
                    link.CalcRailLinkCost(RailLinkCostFunc);
                    link.CalcRailLinkCostPrime(RailLinkCostFuncPrime);
                    link.CalcLinkPrice();
                }

                double sumCost = 0;
                for (auto &&path : pathVec) {
                    path.CalcPathCost(linkVec, demandVec, StayLinkCostFunc, StayLinkCostFunc2);
                    path.CalcPathCostWithPrice(linkVec);
                    sumCost += path.f * path.pathCost;
                }

                for (int k = 0; k < static_cast<int>(pathVec.size()); k++) {
                    pathFlow[k] = pathVec[k].f;
                }
                pathFlowTrans.emplace_back(pathFlow);
                sumCostTrans.emplace_back(sumCost);
                
                CalcMoneyPoint(pathVec, linkVec, nodeVec, demandVec);

                double diff = FlowDynamics(pathVec, demandVec, perturbationTrans);
            }
        }

        std::cout << "With Point終了" << std::endl;
        middle = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(middle - start).count();
        std::cout << elapsed << "秒経過" << std::endl;

        // Without Point
        for (double i = 0; i <= demandVec[0].m; i += 0.5) {
            for (double j = 0; j <= demandVec[0].m - i; j += 0.5) {
                for (auto &&demand : demandVec) {
                    demand.B = 0;
                }
                std::vector<double> pathFlow(pathVec.size(), 0);

                pathVec[0].f = i;
                pathVec[1].f = j;
                pathVec[2].f = demandVec[0].m - i - j;

                for (auto &&link : linkVec) {
                    link.CalcLinkFlow(pathVec);
                    link.CalcRailLinkCost(RailLinkCostFunc);
                    link.CalcRailLinkCostPrime(RailLinkCostFuncPrime);
                    link.CalcLinkPrice();
                }

                double sumCost = 0;
                for (auto &&path : pathVec) {
                    path.CalcPathCost(linkVec, demandVec, StayLinkCostFunc, StayLinkCostFunc2);
                    path.CalcPathCostWithPrice(linkVec);
                    sumCost += path.f * path.pathCost;
                }

                for (int k = 0; k < static_cast<int>(pathVec.size()); k++) {
                    pathFlow[k] = pathVec[k].f;
                }
                pathFlowTransOut.emplace_back(pathFlow);
                sumCostTransOut.emplace_back(sumCost);

                double diff = FlowDynamics(pathVec, demandVec, perturbationTransOut);
            }
        }

        std::cout << "Without Point終了" << std::endl;
        middle = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(middle - start).count();
        std::cout << elapsed << "秒経過" << std::endl;

        // Without Control
        for (double i = 0; i <= demandVec[0].m; i += 0.5) {
            for (double j = 0; j <= demandVec[0].m - i; j += 0.5) {
                for (auto &&demand : demandVec) {
                    demand.B = 0;
                }
                for (auto &&link : linkVec) {
                    link.price = 0;
                    link.point = 0;
                    link.money = 0;
                }
                std::vector<double> pathFlow(pathVec.size(), 0);

                pathVec[0].f = i;
                pathVec[1].f = j;
                pathVec[2].f = demandVec[0].m - i - j;

                double sumCost = 0;
                for (auto &&link : linkVec) {
                    link.CalcLinkFlow(pathVec);
                    link.CalcRailLinkCost(RailLinkCostFunc);
                    link.CalcRailLinkPotential(RailLinkCostFuncIntegral);
                    sumCost -= link.linkCost * link.x;
                    sumCost += link.potential;
                }

                for (auto &&path : pathVec) {
                    path.CalcPathCost(linkVec, demandVec, StayLinkCostFunc, StayLinkCostFunc2);
                    path.CalcPathCostWithPrice(linkVec);
                    sumCost += path.f * path.pathCost;
                }

                for (int k = 0; k < static_cast<int>(pathVec.size()); k++) {
                    pathFlow[k] = pathVec[k].f;
                }
                pathFlowTransNo.emplace_back(pathFlow);
                sumCostTransNo.emplace_back(sumCost);

                double diff = FlowDynamics(pathVec, demandVec, perturbationTransNo);
            }
        }

        std::cout << "Without Control終了" << std::endl;
        middle = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(middle - start).count();
        std::cout << elapsed << "秒経過" << std::endl;

        Write1DFile(sumCostTrans, DIR + SUBDIR + CONTOUR_OBJ_FNAME, "sumCost");
        Write2DFile(pathFlowTrans, DIR + SUBDIR + CONTOUR_FNAME);
        Write2DFile(perturbationTrans, DIR + SUBDIR + CONTOUR_VEC_FNAME);
        Write1DFile(sumCostTransOut, DIR + SUBDIR + "out" + CONTOUR_OBJ_FNAME, "sumCost");
        Write2DFile(pathFlowTransOut, DIR + SUBDIR + "out" + CONTOUR_FNAME);
        Write2DFile(perturbationTransOut, DIR + SUBDIR + "out" + CONTOUR_VEC_FNAME);
        Write1DFile(sumCostTransNo, DIR + SUBDIR + "no" + CONTOUR_OBJ_FNAME, "sumCost");
        Write2DFile(pathFlowTransNo, DIR + SUBDIR + "no" + CONTOUR_FNAME);
        Write2DFile(perturbationTransNo, DIR + SUBDIR + "no" + CONTOUR_VEC_FNAME);
    } else {
        // 流線図用
        std::cout << "流線図用のシミュレーション開始" << std::endl;

        int count = 0;
        for (double i = 0; i <= demandVec[0].m; i += 0.5) {
            for (auto &&demand : demandVec) {
                demand.B = 0;
            }
            std::vector<double> pathFlow(pathVec.size(), 0);

            // 初期解の作成
            pathVec[0].f = i;
            pathVec[1].f = demandVec[0].m - i;
            pathVec[2].f = 0;
            initPathVec = pathVec;
            initPathVec2 = pathVec;

            // With Pointのシミュレーション
            Simulation(pathVec, linkVec, nodeVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTrans, railCostTrans, stayUtilTrans, pathCostTrans, possessedPointTrans, linkFlowTrans, zeroFlowTrans, oneFlowTrans, pathFlowTrans, moneyTrans, pointTrans, railRevenueTrans, storeRevenueTrans, perturbationTrans, pathZeroCostTrans, pathOneCostTrans);

            // Without Pointのシミュレーション
            for (auto &&demand : demandVec) {
                demand.B = 0;
                demand.L = 0;
            }
            SimulationWithout(initPathVec, linkVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTransOut, railCostTransOut, stayUtilTransOut, pathCostTransOut, possessedPointTransOut, linkFlowTransOut, zeroFlowTransOut, oneFlowTransOut, pathFlowTransOut, moneyTransOut, pointTransOut, railRevenueTransOut, storeRevenueTransOut, perturbationTransOut, pathZeroCostTransOut, pathOneCostTransOut);

            // Without Controlのシミュレーション
            for (auto &&demand : demandVec) {
                demand.B = 0;
                demand.L = 0;
            }
            for (auto &&link : linkVec) {
                link.price = 0;
                link.money = 0;
                link.point = 0;
            }
            SimulationNoControl(initPathVec2, linkVec, nodeVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTransNo, railCostTransNo, stayUtilTransNo, pathCostTransNo, possessedPointTransNo, linkFlowTransNo, zeroFlowTransNo, oneFlowTransNo, pathFlowTransNo, moneyTransNo, pointTransNo, railRevenueTransNo, storeRevenueTransNo, perturbationTransNo, pathZeroCostTransNo, pathOneCostTransNo);


            // perturbationTransは最終日が記録されていないので記録する
            perturbationTrans.emplace_back(std::vector<double>(pathVec.size(), 0));
            perturbationTransOut.emplace_back(std::vector<double>(pathVec.size(), 0));
            perturbationTransNo.emplace_back(std::vector<double>(pathVec.size(), 0));

            count++;
            std::cout << count << "回目のシミュレーション終了" << std::endl;
            middle = std::chrono::system_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::seconds>(middle - start).count();
            std::cout << elapsed << "秒経過" << std::endl;


            for (auto &&demand : demandVec) {
                demand.B = 0;
            }
            pathFlow = std::vector<double>(pathVec.size(), 0);

            pathVec[0].f = 0;
            pathVec[1].f = i;
            pathVec[2].f = demandVec[0].m - i;
            initPathVec = pathVec;
            initPathVec2 = pathVec;

            Simulation(pathVec, linkVec, nodeVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTrans, railCostTrans, stayUtilTrans, pathCostTrans, possessedPointTrans, linkFlowTrans, zeroFlowTrans, oneFlowTrans, pathFlowTrans, moneyTrans, pointTrans, railRevenueTrans, storeRevenueTrans, perturbationTrans, pathZeroCostTrans, pathOneCostTrans);

            for (auto &&demand : demandVec) {
                demand.B = 0;
                demand.L = 0;
            }
            SimulationWithout(initPathVec, linkVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTransOut, railCostTransOut, stayUtilTransOut, pathCostTransOut, possessedPointTransOut, linkFlowTransOut, zeroFlowTransOut, oneFlowTransOut, pathFlowTransOut, moneyTransOut, pointTransOut, railRevenueTransOut, storeRevenueTransOut, perturbationTransOut, pathZeroCostTransOut, pathOneCostTransOut);

            for (auto &&demand : demandVec) {
                demand.B = 0;
                demand.L = 0;
            }
            for (auto &&link : linkVec) {
                link.price = 0;
                link.money = 0;
                link.point = 0;
            }
            SimulationNoControl(initPathVec2, linkVec, nodeVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTransNo, railCostTransNo, stayUtilTransNo, pathCostTransNo, possessedPointTransNo, linkFlowTransNo, zeroFlowTransNo, oneFlowTransNo, pathFlowTransNo, moneyTransNo, pointTransNo, railRevenueTransNo, storeRevenueTransNo, perturbationTransNo, pathZeroCostTransNo, pathOneCostTransNo);

            // perturbationTransは最終日が記録されていないので記録する
            perturbationTrans.emplace_back(std::vector<double>(pathVec.size(), 0));
            perturbationTransOut.emplace_back(std::vector<double>(pathVec.size(), 0));
            perturbationTransNo.emplace_back(std::vector<double>(pathVec.size(), 0));

            count++;
            std::cout << count << "回目のシミュレーション終了" << std::endl;
            middle = std::chrono::system_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::seconds>(middle - start).count();
            std::cout << elapsed << "秒経過" << std::endl;


            for (auto &&demand : demandVec) {
                demand.B = 0;
            }
            pathFlow = std::vector<double>(pathVec.size(), 0);

            pathVec[0].f = i;
            pathVec[1].f = 0;
            pathVec[2].f = demandVec[0].m - i;
            initPathVec = pathVec;
            initPathVec2 = pathVec;

            Simulation(pathVec, linkVec, nodeVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTrans, railCostTrans, stayUtilTrans, pathCostTrans, possessedPointTrans, linkFlowTrans, zeroFlowTrans, oneFlowTrans, pathFlowTrans, moneyTrans, pointTrans, railRevenueTrans, storeRevenueTrans, perturbationTrans, pathZeroCostTrans, pathOneCostTrans);

            for (auto &&demand : demandVec) {
                demand.B = 0;
                demand.L = 0;
            }
            SimulationWithout(initPathVec, linkVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTransOut, railCostTransOut, stayUtilTransOut, pathCostTransOut, possessedPointTransOut, linkFlowTransOut, zeroFlowTransOut, oneFlowTransOut, pathFlowTransOut, moneyTransOut, pointTransOut, railRevenueTransOut, storeRevenueTransOut, perturbationTransOut, pathZeroCostTransOut, pathOneCostTransOut);

            for (auto &&demand : demandVec) {
                demand.B = 0;
                demand.L = 0;
            }
            for (auto &&link : linkVec) {
                link.price = 0;
                link.money = 0;
                link.point = 0;
            }
            SimulationNoControl(initPathVec2, linkVec, nodeVec, demandVec, RailLinkCostFunc, RailLinkCostFuncPrime, StayLinkCostFunc, StayLinkCostFunc2, sumCostTransNo, railCostTransNo, stayUtilTransNo, pathCostTransNo, possessedPointTransNo, linkFlowTransNo, zeroFlowTransNo, oneFlowTransNo, pathFlowTransNo, moneyTransNo, pointTransNo, railRevenueTransNo, storeRevenueTransNo, perturbationTransNo, pathZeroCostTransNo, pathOneCostTransNo);


            // perturbationTransは最終日が記録されていないので記録する
            perturbationTrans.emplace_back(std::vector<double>(pathVec.size(), 0));
            perturbationTransOut.emplace_back(std::vector<double>(pathVec.size(), 0));
            perturbationTransNo.emplace_back(std::vector<double>(pathVec.size(), 0));

            count++;
            std::cout << count << "回目のシミュレーション終了" << std::endl;
            middle = std::chrono::system_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::seconds>(middle - start).count();
            std::cout << elapsed << "秒経過" << std::endl;

        }

        Write2DFile(pathFlowTrans, DIR + SUBDIR + STREAM_FNAME);
        Write2DFile(perturbationTrans, DIR + SUBDIR + STREAM_VEC_FNAME);
        Write2DFile(pathFlowTransOut, DIR + SUBDIR + "out" + STREAM_FNAME);
        Write2DFile(perturbationTransOut, DIR + SUBDIR + "out" + STREAM_VEC_FNAME);
        Write2DFile(pathFlowTransNo, DIR + SUBDIR + "no" + STREAM_FNAME);
        Write2DFile(perturbationTransNo, DIR + SUBDIR + "no" + STREAM_VEC_FNAME);
    }

    std::cout << "プログラム終了" << std::endl;
    end = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << elapsed << "秒経過" << std::endl;

    return 0;
}