#pragma once
#include "common.hpp"
#include "../rapidjson/document.h"
#include "../rapidjson/istreamwrapper.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/ostreamwrapper.h"

using namespace rapidjson;
class jsonInput // 헤더파일이므로 선언만 한다.
{
private:
    std::string local_host; // 멤버변수 생성
    const char *local_port;
    std::string forward_host;
    const char *forward_port;

public:
    int start(); // start 메소드

    std::string GetLocalhost(); // 각 값을 멤버변수에 담아주는 메소드
    const char *GetLocalport();
    std::string GetForwardhost();
    const char *GetForwardport();
};