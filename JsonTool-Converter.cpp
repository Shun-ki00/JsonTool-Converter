#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void ShowUsage()
{
    std::cout << "Usage: Converter.exe <input.json> <format>\n";
    std::cout << "Supported formats: msgpack, cbor, bson\n";
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        ShowUsage();
        return 1;
    }

    const std::string inputPath = argv[1];
    const std::string format = argv[2];

    // 入力ファイル読み込み
    std::ifstream inputFile(inputPath);
    if (!inputFile)
    {
        std::cerr << "Failed to open input file: " << inputPath << '\n';
        return 1;
    }

    json j;
    try
    {
        inputFile >> j;
    }
    catch (const std::exception& e)
    {
        std::cerr << "JSON parse error: " << e.what() << '\n';
        return 1;
    }

    std::vector<uint8_t> binary;

    // フォーマットに応じたバイナリ変換
    if (format == "msgpack")
        binary = json::to_msgpack(j);
    else if (format == "cbor")
        binary = json::to_cbor(j);
    else if (format == "bson")
        binary = json::to_bson(j);
    else
    {
        std::cerr << "Unsupported format: " << format << '\n';
        ShowUsage();
        return 1;
    }

    // 出力ファイル名決定
    std::string outputPath = inputPath.substr(0, inputPath.find_last_of('.')) + "." + format;

    // 書き出し
    std::ofstream outputFile(outputPath, std::ios::binary);
    outputFile.write(reinterpret_cast<const char*>(binary.data()), binary.size());

    std::cout << "Converted to: " << outputPath << '\n';

    return 0;
}