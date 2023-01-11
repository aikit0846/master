#ifndef INPUT_HPP_
#define INPUT_HPP_

#include "global.hpp"

// カンマで分割
std::vector<std::string> split(std::string &input, char delimeter);

// ノードファイル読み込み用関数
std::vector<Node> ReadNodeFile(std::string nodeFName);

// リンクファイル読み込み用関数
std::vector<Link> ReadLinkFile(std::string linkFName);

// 需要情報ファイル読み込み用関数
std::vector<Demand> ReadDemandFile(std::string demandFName);

// 経路ファイル読み込み用関数
std::vector<Path> ReadPathFile(std::string pathFName, std::vector<Link> &linkVec, std::vector<Demand> &demandVec);

#endif