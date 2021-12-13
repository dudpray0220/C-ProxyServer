#include "../inc/bridge.hpp"
// public

namespace yhbae {

bridge::bridge(boost::asio::io_service &ios)          // 생성자
    : downstream_socket_(ios), upstream_socket_(ios)  // 멤버변수 down, upstream_socket에 파라미터를 연결해줌.
      {};

bridge::socket_type &yhbae::bridge::downstream_socket()  // &를 붙이면 주소가 된다. 멤버함수(Method), socket_type은 return값의 타입이다.
{
    // Client socket
    return downstream_socket_;
};

bridge::socket_type &yhbae::bridge::upstream_socket() {
    // Remote server socket
    return upstream_socket_;
};

// start 함수
void bridge::start(const std::string &upstream_host, unsigned short upstream_port) {  // void는 return이 없음. string&는 upstream_host의 메모리 주소를 참조하는 것.
    // Attempt connection to remote server (upstream side)
    upstream_socket_.async_connect(  // async_connect는 소켓을 지정된 원격 엔드포인트로 비동기 연결하는데 사용 (비동기 연결을 시작한다.)
        ip::tcp::endpoint(
            boost::asio::ip::address::from_string(upstream_host),  // async_connect의 첫번째 파라미터
            upstream_port),

        // async_connect의 두번째 파라미터
        // bind는 주어진 엔드포인터로 소켓을 바인드한다.
        boost::bind(&bridge::handle_upstream_connect,  // boost::bind() 에게 첫번째로 들어갈 인자는 바인딩할 함수의 주소. handle_upstream_connect의 메모리 주소를 참조.
                                                       // handle_upstream_connect함수를 start에 바인딩?
                    shared_from_this(),                // 그 다음의 인자들 바인딩될시 매핑될 매개 변수 리스트들
                    boost::asio::placeholders::error));
};
// handle_upstream_connect 함수
void bridge::handle_upstream_connect(const boost::system::error_code &error) {
    if (!error) {
        // Setup async read from remote server (upstream)
        upstream_socket_.async_read_some(  // 비동기 읽기를 시작한다. 스트림 소켓에서 데이터를 비동기로 읽는데 사용된다.
            boost::asio::buffer(upstream_data_, max_data_length),
            boost::bind(&bridge::handle_upstream_read,  // 서버로부터 데이터를 읽어옴
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));

        // Setup async read from client (downstream)
        downstream_socket_.async_read_some(  // 비동기 읽기를 시작한다. 스트림 소켓에서 데이터를 비동기로 읽는데 사용된다.
            boost::asio::buffer(downstream_data_, max_data_length),
            boost::bind(&bridge::handle_downstream_read,  // 클라이언트로부터 데이터를 읽어옴
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else
        close();  // error시 종료
};

// private
/*
   Section A: Remote Server --> Proxy --> Client (다운스트림)
   Process data recieved from remote sever then send to client. (서버로부터 데이터를 받아서 클라이언트에 보낸다)
*/

// Read from remote server complete, now send data to client (서버로부터 데이터를 읽기 complete, 클라이언트로 데이터를 보낸다)
void bridge::handle_upstream_read(const boost::system::error_code &error, const size_t &bytes_transferred) {
    if (!error) {
        async_write(downstream_socket_,  // async_write는 스트림에 제공된 모든 데이터를 쓰는 비동기 작업을 시작한다.
                    boost::asio::buffer(upstream_data_, bytes_transferred),
                    boost::bind(&bridge::handle_downstream_write,
                                shared_from_this(),
                                boost::asio::placeholders::error));
    } else
        close();
};

// Write to client complete, Async read from remote server (클라이언트에 데이터를 쓰기 complete, 서버데이터를 비동기로 읽는다)
void bridge::handle_downstream_write(const boost::system::error_code &error) {
    if (!error) {
        upstream_socket_.async_read_some(
            boost::asio::buffer(upstream_data_, max_data_length),
            boost::bind(&bridge::handle_upstream_read,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else
        close();
};

// *** End Of Section A *** (다운스트림 완료)

/*
   Section B: Client --> Proxy --> Remove Server (업스트림)
   Process data recieved from client then write to remove server. (클라이언트로부터 데이터를 받아 서버에 쓴다)
*/

// Read from client complete, now send data to remote server (클라이언트로부터 데이터를 읽기 complete, 서버로 데이터를 보낸다)
void bridge::handle_downstream_read(const boost::system::error_code &error, const size_t &bytes_transferred) {
    if (!error) {
        async_write(upstream_socket_,
                    boost::asio::buffer(downstream_data_, bytes_transferred),
                    boost::bind(&bridge::handle_upstream_write,
                                shared_from_this(),
                                boost::asio::placeholders::error));
    } else
        close();
};

// Write to remote server complete, Async read from client (서버에 데이터를 쓰기 complete, 클라이언트데이터를 비동기로 읽는다)
void bridge::handle_upstream_write(const boost::system::error_code &error) {
    if (!error) {
        downstream_socket_.async_read_some(
            boost::asio::buffer(downstream_data_, max_data_length),
            boost::bind(&bridge::handle_downstream_read,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else
        close();
};

// *** End Of Section B *** (업스트림 완료)

void bridge::close() {  // 종료함수
    boost::mutex::scoped_lock lock(mutex_);

    if (downstream_socket_.is_open())  // 소켓이 만약 열려있으면 닫는다. downstream_socket_은 즉, client socket
    {
        downstream_socket_.close();
    }

    if (upstream_socket_.is_open()) {  // upstream_socket_은 즉, Remote server socket
        upstream_socket_.close();
    }
}
}  // namespace yhbae
