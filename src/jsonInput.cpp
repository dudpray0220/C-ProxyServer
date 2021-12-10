#include "../inc/jsonInput.hpp"

// Using rapidjson library
int jsonInput::start()  // start 메소드
{
    std::ifstream ifs{R"(../info.json)"};  // ifs에 json파일을 읽어들인다.
    if (!ifs.is_open()) {                  // 못 읽어들일시 error 출력 (cerr : error에 대한 표준 출력 스트림)
        std::cerr << "Could not open file for reading!\n";
        return EXIT_FAILURE;
    }

    IStreamWrapper isw{ifs};

    Document doc{};        // Document는 GenericDocument<UTF8<> > 의 typedef.
    doc.ParseStream(isw);  // 읽은 json 파일을 parsing 해줌.

    StringBuffer buffer{};  // StringBuffer는 GenericStringBuffer<UTF8<> >의 typedef. json을 쓰기위한 메모리버퍼 할당. GetString()버퍼를 얻는데 사용.
    Writer<StringBuffer> writer{buffer};
    doc.Accept(writer);

    if (doc.HasParseError()) {  // Parse에서 error 발생 시 error 출력
        std::cout << "Error  : " << doc.GetParseError() << '\n'
                  << "Offset : " << doc.GetErrorOffset() << '\n';
        return EXIT_FAILURE;
    }

    const std::string jsonStr{buffer.GetString()};  // jsonStr에 json 값을 넣는다.

    local_host = doc["local_host"].GetString();  // 각각 맞는 값을 멤버변수에 담아줌.
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

// json에서 원하는 값을 return 시키는 method
std::string jsonInput::GetLocalhost() {
    return local_host;
};
const char *jsonInput::GetLocalport() {
    return local_port;
};
std::string jsonInput::GetForwardhost() {
    return forward_host;
};
const char *jsonInput::GetForwardport() {
    return forward_port;
};