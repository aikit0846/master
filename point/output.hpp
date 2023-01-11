#ifndef OUTPUT_HPP_
#define OUTPUT_HPP_

#include "global.hpp"

// 総コスト記録用
void WriteSumCostFile(std::vector<double> sumCostTrans, std::string sumCostName);

// 状態とコスト記録用
void WriteXCTransFile(std::vector<std::vector<std::vector<double>>> xTrans, std::vector<std::vector<std::vector<double>>> cTrans, std::string xTransName, std::string cTransName);

// 待ち行列の有無記録用
void WriteOverTransFile(std::vector<bool> overTrans, std::string overTransName);

// パラメータ記録用
void WriteParameterFile();

#endif