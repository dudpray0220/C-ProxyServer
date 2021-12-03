#pragma once
#include "common.hpp"
#include "../rapidjson/document.h"
#include "../rapidjson/istreamwrapper.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/ostreamwrapper.h"

using namespace rapidjson;
class JsonInput
{
private:
    std::string local_host; // 멤버변수 생성
    const char *local_port;
    std::string forward_host;
    const char *forward_port;

public:
    int start() // start 메소드
    {
        std::ifstream ifs{R"(../info.json)"};
        if (!ifs.is_open())
        {
            std::cerr << "Could not open file for reading!\n";
            return EXIT_FAILURE;
        }

        IStreamWrapper isw{ifs};

        Document doc{};
        doc.ParseStream(isw);

        StringBuffer buffer{};
        Writer<StringBuffer> writer{buffer};
        doc.Accept(writer);

        if (doc.HasParseError())
        {
            std::cout << "Error  : " << doc.GetParseError() << '\n'
                      << "Offset : " << doc.GetErrorOffset() << '\n';
            return EXIT_FAILURE;
        }

        const std::string jsonStr{buffer.GetString()}; // jsonStr에 json 값이 담겼다.

        local_host = doc["local_host"].GetString(); // 각각 맞는 값을 멤버변수에 담아줌.
        local_port = doc["local_port"].GetString();
        forward_host = doc["forward_host"].GetString();
        forward_port = doc["forward_port"].GetString();
        // std::cout << jsonStr << '\n';
        // std::cout << doc["local_host"].GetString() << std::endl;      // local_host의 값이 출력된다.
        // std::cout << doc["local_port"].GetString() << std::endl; // local_port의 값이 출력된다.
        // std::cout << jsonStr[14] << '\n'; // jsonStr[10] = s, jsonStr[11] = t 이다.

        // doc["local_host"] = "127.0.0.1"; // 이렇게 하면 아예 json에서 local_host 값이 바뀜

        // std::ofstream ofs{R"(../info.json)"};
        // if (!ofs.is_open())
        // {
        //     std::cerr << "Could not open file for writing!\n";
        //     return EXIT_FAILURE;
        // }

        // OStreamWrapper osw{ofs};
        // Writer<OStreamWrapper> writer2{osw};
        // doc.Accept(writer2);

        // return EXIT_SUCCESS;
        return 0;
    };

    std::string GetLocalhost()
    {
        return local_host;
    };
    const char *GetLocalport()
    {
        return local_port;
    };
    std::string GetForwardhost()
    {
        return forward_host;
    };
    const char *GetForwardport()
    {
        return forward_port;
    };
};