#include "../inc/acceptor.hpp"

// public
acceptor::acceptor(boost::asio::io_service &io_service,  // 생성자 (만드려면 인자가 5개 필요하다)
                   const std::string &local_host, unsigned short local_port,
                   const std::string &upstream_host, unsigned short upstream_port)
    : io_service_(io_service),
      localhost_address(boost::asio::ip::address_v4::from_string(local_host)),
      acceptor_(io_service_, ip::tcp::endpoint(localhost_address, local_port)),
      upstream_port_(upstream_port),
      upstream_host_(upstream_host){};

bool acceptor::accept_connections() {
    try {
        session_ = boost::shared_ptr<bridge>(new bridge(io_service_));  // 동적할당

        acceptor_.async_accept(session_->downstream_socket(),  // downstream_socket이란 method 호출, async_accept 내에서 throw를 날림.
                               boost::bind(&acceptor::handle_accept,
                                           this,
                                           boost::asio::placeholders::error));
    } catch (std::exception &e) {
        std::cerr << "acceptor exception: " << e.what() << std::endl;
        return false;
    }

    return true;
};

ip::address_v4 acceptor::abc() {
    return localhost_address;
}

// private
void acceptor::handle_accept(const boost::system::error_code &error) {
    if (!error) {
        session_->start(upstream_host_, upstream_port_);  // start 함수 호출

        if (!accept_connections())  // accept_connections이 false면
        {
            std::cerr << "Failure during call to accept." << std::endl;
        }
    } else  // error면
    {
        std::cerr << "Error: " << error.message() << std::endl;
    }
};
