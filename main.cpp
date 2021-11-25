#include "./inc/tcpproxy_server.hpp"

// argc 는 argument의 수를 의미한다. argv 는 argument 가 char 형으로 저장이 되는 변수.
// argv[0] 은 항상 실행 파일명(실행 경로)이 저장된다. argv[1], argv[2] … 에는 순서대로 사용자가 입력한 argument 가 저장된다.

int main(int argc, char *argv[])
{
   if (argc != 5) // argv[1] ~ argv[4] : local host ip ~ forward port까지 들어간다. argc에는 그 갯수인 5가 들어간다.
   {
      std::cerr << "usage: tcpproxy_server <local host ip> <local port> <forward host ip> <forward port>" << std::endl;
      return 1; // 정상종료 but 무엇인가 있음.
   }

   const unsigned short local_port = static_cast<unsigned short>(::atoi(argv[2])); // static_cast<바꾸려고 하는 타입>(대상);
   const unsigned short forward_port = static_cast<unsigned short>(::atoi(argv[4]));
   const std::string local_host = argv[1];
   const std::string forward_host = argv[3];

   boost::asio::io_service ios;

   try
   {
      yhbae::bridge::acceptor acceptor(ios,
                                       local_host, local_port,
                                       forward_host, forward_port);

      acceptor.accept_connections();

      ios.run();
   }
   catch (std::exception &e)

   {
      std::cerr << "Error: " << e.what() << std::endl;
      return 1;
   }

   return 0;
}