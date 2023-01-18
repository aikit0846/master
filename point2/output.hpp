#ifndef OUTPUT_HPP_
#define OUTPUT_HPP_

#include "global.hpp"

// 2次元ベクター出力用
void Write1DFile(std::vector<double> vec, std::string fName, std::string colName);

// 2次元ベクター出力用
void Write2DFile(std::vector<std::vector<double>> vec, std::string fName);

// パラメータ記録用
void WriteParameterFile(std::string SUBDIR);

#endif