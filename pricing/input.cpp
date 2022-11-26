#include "input.hpp"


// カンマで分割する
std::vector<std::string> split(std::string &input, char delimeter)
{
    std::istringstream stream(input);
    std::string field;
    std::vector<std::string> result;

    while (getline(stream, field, delimeter)) {
        result.emplace_back(field);
    }

    return result;
}

// 施設ファイル読み込み用関数
std::vector<std::vector<SpatialLink>> ReadNetworkFile(std::string nw_path)
{
    std::ifstream ifs(nw_path);
    std::string line;
    std::vector<SpatialLink> SL;
    std::vector<std::vector<SpatialLink>> Link(T_BAR + 1);
    int t, count, i;

    count = 0;
    while (getline(ifs, line)) {
        // 1行目はindexなので飛ばす
        if (count == 0) {
            count++;
            continue;
        }

        std::vector<std::string> strvec = split(line, ',');
        SpatialLink sl;
        
        sl.spaceIdx = std::stoi(strvec[0]);
        sl.o = std::stoi(strvec[1]);
        sl.d = std::stoi(strvec[2]);
        sl.t = std::stoi(strvec[3]);

        SL.emplace_back(sl);
    }

    for (t = 0; t <= T_BAR; t++) {
        Link[t] = SL;
        for (i = 0; i < static_cast<int>(Link[t].size()); i++) {
            Link[t][i].timeIdx = t;
        }
    }

    return Link;
}


// 集団情報ファイル読み込み用関数
std::vector<Population> ReadPopulationFile(std::string pop_path)
{
    std::ifstream ifs(pop_path);
    std::string line;
    std::vector<Population> P;
    int i, count;

    count = 0;
    while (getline(ifs, line)) {
        // 1行目は列名なので飛ばす
        if (count == 0) {
            count++;
            continue;
        }

        std::vector<std::string> strvec = split(line, ',');
        Population p;

        p.id = std::stoi(strvec[0]);
        p.o = std::stoi(strvec[1]);
        p.d = std::stoi(strvec[2]);
        p.m = std::stoi(strvec[3]);

        P.emplace_back(p);
    }

    return P;
}