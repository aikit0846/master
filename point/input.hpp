#ifndef INPUT_HPP_
#define INPUT_HPP_

#include "global.hpp"

// カンマで分割
std::vector<std::string> split(std::string &input, char delimeter);

// 施設ファイル読み込み用関数
std::vector<std::vector<SpatialLink>> ReadNetworkFile(std::string nw_path);

// 集団情報ファイル読み込み用関数
std::vector<Population> ReadPopulationFile(std::string pop_path);

#endif