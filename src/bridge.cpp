#include "../inc/bridge.hpp"
// public
yhbae::bridge::bridge(boost::asio::io_service &ios)  // 생성자
    : downstream_socket_(ios), upstream_socket_(ios) // 멤버변수 down, upstream_socket에 파라미터를 연결해줌.
      {};