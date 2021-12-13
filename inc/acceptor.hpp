#pragma once
#include "bridge.hpp"

namespace yhbae {

class acceptor  // 헤더파일이므로 선언만 한다.
{
   public:
    acceptor(boost::asio::io_service &io_service,  // 생성자 (만드려면 인자가 5개 필요하다)
             const std::string &local_host, unsigned short local_port,
             const std::string &upstream_host, unsigned short upstream_port);

    // accept_connections 함수, bool은 이 함수의 return type이다.
    bool accept_connections();

   private:
    // handle_accept 함수, void는 return이 없음.
    void handle_accept(const boost::system::error_code &error);

    // 멤버변수 선언 (in Private)
    boost::asio::io_service &io_service_;  // io_service는 OS의 리소스에 접근하고 i/o 요청을 수행하는 프로그램을 OS와 상호작용할 수 있도록 중개하는 역할.
    ip::address_v4 localhost_address;
    ip::tcp::acceptor acceptor_;
    yhbae::bridge::ptr_type session_;
    unsigned short upstream_port_;
    std::string upstream_host_;
    int32_t mCount;  // 클라이언트 연결 수 제한을 위한 멤버변수
};

}  // namespace yhbae