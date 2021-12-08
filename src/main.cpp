#include "../inc/acceptor.hpp"
#include "../inc/bridge.hpp"
#include "../inc/jsonInput.hpp"

// argc 는 argument의 수를 의미한다. argv 는 argument 가 char 형으로 저장이 되는 변수.
// argv[0] 은 항상 실행 파일명(실행 경로)이 저장된다. argv[1], argv[2] … 에는 순서대로 사용자가 입력한 argument 가 저장된다.

int main(int argc, char *argv[]) {
    // if (argc != 5) // argv[1] ~ argv[4] : local host ip ~ forward port까지 들어간다. argc에는 그 갯수인 5가 들어간다.
    // {
    //    std::cerr << "usage: tcpproxy_server <local host ip> <local port> <forward host ip> <forward port>" << std::endl; // 즉 argc가 5가 아니면 출력.
    //    return 1;                                                                                                         // return 1 : 정상종료 but 무엇인가 있음.
    // }

    // JsonInput Class 호출
    jsonInput jInput;
    jInput.start();  // json파일 읽어와서 멤버변수 4개에 값을 알맞게 담는다.

    // 한 이쯤 rapidjson으로 불러와야함. unsigned short는 ~65,535. 즉, 포트범위.
    const std::string local_host = jInput.GetLocalhost();
    const std::string forward_host = jInput.GetForwardhost();

    const unsigned short local_port = static_cast<unsigned short>(::atoi(jInput.GetLocalport()));      // static_cast<바꾸려고 하는 타입>(대상);
    const unsigned short forward_port = static_cast<unsigned short>(::atoi(jInput.GetForwardport()));  // atoi는 문자열을 정수로 바꿔줌.

    // 테스트 출력
    // std::cout << local_host << std::endl;
    // std::cout << forward_host << std::endl;
    // std::cout << local_port << std::endl;
    // std::cout << forward_port << std::endl;

    boost::asio::io_service ios;

    try {
        yhbae::acceptor acceptor(ios,
                                 local_host, local_port,
                                 forward_host, forward_port);  // acceptor 생성자를 이용하여 객체생성

        acceptor.accept_connections();  // accept_connections 함수 호출

        ios.run();
    } catch (std::exception &e)

    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}