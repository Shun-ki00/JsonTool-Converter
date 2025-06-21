#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


void ShowUsage()
{
    std::cout << "Usage: Converter.exe <input.json> <format>" << std::endl;
    std::cout << "Supported formats: msgpack, cbor, bson" << std::endl;
}

bool ConvertJsonToBinary(const std::filesystem::path& inputPath, const std::string& format, std::filesystem::path& outputPath)
{
    std::ifstream inputFile(inputPath);

    // ファイルが開けない場合
    if (!inputFile)
    {
        std::cerr << "ファイルが開けません" << std::endl;
        return false;
    }

    json j;
    try { inputFile >> j; }
    catch (const std::exception& e)
    {
        std::cerr << "JSONに変換できませんでした：" << std::endl;
        return false;
    }

    // 各指定のフォーマットで変換を行う
    std::vector<uint8_t> binary;
    if (format == "msgpack")
        binary = json::to_msgpack(j);
    else if (format == "cbor")
        binary = json::to_cbor(j);
    else if (format == "bson")
        binary = json::to_bson(j);
    else
    {
        std::cerr << "このフォーマットはサポートされていません :" << format << '\n';
        return false;
    }

    // 指定のフォルダに保存する
    std::filesystem::path outputFile = outputPath / (inputPath.stem().string() + "." + format);
    std::ofstream output(outputFile, std::ios::binary);
    output.write(reinterpret_cast<const char*>(binary.data()), binary.size());

    // 変換、出力が成功したログ
    std::cout << "Converted: " << inputPath.filename() << " -> " << outputFile.filename() << '\n';

    return true;
}


int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        ShowUsage();
        return 1;
    }


    // 入力された値
    const std::filesystem::path inputRootPath = argv[1];
    const std::string format = argv[2];
          std::filesystem::path outputPath = argv[3];

    // ファイルパスの確認
    if (!std::filesystem::is_regular_file(inputRootPath) && !std::filesystem::is_directory(inputRootPath))
    {
        // エラー表示
        std::cerr << "パスが存在しません :" << inputRootPath << std::endl;
    }

    // ファイルの作成
    std::filesystem::create_directories(outputPath);

    // 変換を行う
    for (const auto& entry : std::filesystem::directory_iterator(inputRootPath))
    {
        // 拡張子が「.json」の場合変換を行う
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            // 変換を行う
            ConvertJsonToBinary(entry.path(), format, outputPath);
        }
    }

    return 0;
}