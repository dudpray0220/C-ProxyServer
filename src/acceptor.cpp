#include "../inc/acceptor.hpp"

namespace yhbae {
// public
acceptor::acceptor(boost::asio::io_service &io_service,  // 생성자 (만드려면 인자가 5개 필요하다)
                   const std::string &local_host, unsigned short local_port,
                   const std::string &upstream_host, unsigned short upstream_port)
    : io_service_(io_service),
      localhost_address(boost::asio::ip::address_v4::from_string(local_host)),
      acceptor_(io_service_, ip::tcp::endpoint(localhost_address, local_port)),
      upstream_port_(upstream_port),
      upstream_host_(upstream_host),
      mAttempt(0){};  // 생성자 만든 후 항상 멤버변수 지정을 해줘야한다!

// accept_connections Method
bool acceptor::accept_connections() {
    try {
        std::cout << "mAttempt : " << mAttempt << std::endl;  // mAttempt : 0 1 2 3 ~
        mAttempt++;                                           // 처음 실행시 0에서 ++하여 mAttempt=1,

        if (mAttempt < 10) {                                                // 10번째 시도부터 안되게 함.
            session_ = boost::shared_ptr<bridge>(new bridge(io_service_));  // session_의 뜻 : io_service를 얼마나 참조하고 있느냐
            // 동적할당. shared_ptr는 특정 자원을 가리키는 참조 카운트를 유지하고 있다가 이것이 0이 되면 해당 자원을 자동으로 delete해주는 스마트 포인터
            // async_accept는 비등기 승인을 시작한다.
            acceptor_.async_accept(session_->downstream_socket(),         // downstream_socket이란 method 호출 -> client socket을 return,
                                                                          //  try catch문에선 async_accept 내에서 throw를 날림. throw에 걸리면 catch, 아니면 try문 수행
                                   boost::bind(&acceptor::handle_accept,  // acceptor class에 속한 handle_accept 함수를 파라미터로 넣음.
                                               this,
                                               boost::asio::placeholders::error));
        } else {
            std::cerr << "Limit number of client connections" << std::endl;  // mAttempt : 9 다음 error 출력. 접속 막음.
        }

    } catch (std::exception &e) {
        std::cerr << "acceptor exception: " << e.what() << std::endl;
        return false;
    }
    return true;
};

// private
// handle_accept Method
void acceptor::handle_accept(const boost::system::error_code &error) {
    if (!error) {                                         // 에러가 아닐시
        session_->start(upstream_host_, upstream_port_);  // start 함수 호출 (Attempt connection to remote server (upstream side))
        if (!accept_connections())                        // accept_connections이 false면 error 출력
        {
            std::cerr << "Failure during call to accept." << std::endl;
        }
    } else {  // 에러일시
        std::cerr << "Error: " << error.message() << std::endl;
    }
};

}  // namespace yhbae