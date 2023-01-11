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


// ノードファイル読み込み
std::vector<Node> ReadNodeFile(std::string nodeFName)
{
    std::ifstream ifs(nodeFName);
    std::string line;
    std::vector<Node> nodeVec;
    std::vector<std::string> strvec;
    int i;

    i = 0;
    while (getline(ifs, line)) {
        // 1行目は列名なので飛ばす
        if (i == 0) {
            i++;
            continue;
        }

        strvec = split(line, ',');
        Node node;

        node.idx = std::stoi(strvec[0]);
        node.i = std::stoi(strvec[1]);
        node.t = std::stoi(strvec[2]);
        node.type = strvec[3][0];

        nodeVec.emplace_back(node);
    }

    return nodeVec;
}


// リンクファイル読み込み用関数
std::vector<Link> ReadLinkFile(std::string linkFName)
{
    std::ifstream ifs(linkFName);
    std::string line;
    std::vector<Link> linkVec;
    std::vector<std::string> strvec;
    int i;

    i = 0;
    while (getline(ifs, line)) {
        // 1行目は列名なので飛ばす
        if (i == 0) {
            i++;
            continue;
        }

        strvec = split(line, ',');
        Link link;
        
        link.idx = std::stoi(strvec[0]);
        link.oidx = std::stoi(strvec[1]);
        link.didx = std::stoi(strvec[2]);
        link.type = strvec[3][0];
        link.poiU = std::stod(strvec[4]);

        linkVec.emplace_back(link);
    }

    return linkVec;
}


// 集団情報ファイル読み込み用関数
std::vector<Demand> ReadDemandFile(std::string demandFName)
{
    std::ifstream ifs(demandFName);
    std::string line;
    std::vector<Demand> demandVec;
    std::vector<std::string> strvec;
    int i;

    i = 0;
    while (getline(ifs, line)) {
        // 1行目は列名なので飛ばす
        if (i == 0) {
            i++;
            continue;
        }

        strvec = split(line, ',');
        Demand demand;

        demand.idx = std::stoi(strvec[0]);
        demand.o = std::stoi(strvec[1]);
        demand.d = std::stoi(strvec[2]);
        demand.m = std::stoi(strvec[3]);

        demandVec.emplace_back(demand);
    }

    return demandVec;
}


// 経路情報読み込み
std::vector<Path> ReadPathFile(std::string pathFName, std::vector<Link> &linkVec, std::vector<Demand> &demandVec)
{
    std::ifstream ifs(pathFName);
    std::string line;
    std::vector<Path> pathVec;
    std::vector<std::string> strvec;
    int t, pathNum, count;

    count = 0;
    while (getline(ifs, line)) {
        if (count == 0) {
            count++;
            continue;
        }

        strvec = split(line, ',');
        Path path;

        path.idx = std::stoi(strvec[0]);
        path.odidx = std::stoi(strvec[1]);
        demandVec[path.odidx].pathIdx.emplace_back(path.idx);  // 需要データとの紐付け
        path.boardNum = 0;
        for (t = 2; t <= 2 + T_BAR; t++) {
            path.linkSeries.emplace_back(std::stoi(strvec[t]));
            linkVec[std::stoi(strvec[t])].inPathIdx.emplace_back(path.idx);  // リンクデータとの紐付け
            // 乗車数のカウント
            if (t > 2 && linkVec[std::stoi(strvec[t - 1])].type != 'r' && linkVec[std::stoi(strvec[t])].type == 'r') {
                path.boardNum++;
            }
        }

        pathVec.emplace_back(path);
    }

    return pathVec;
}