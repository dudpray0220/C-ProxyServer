#pragma once  // 헤더가드
#include "common.hpp"

namespace yhbae {  // 헤더파일이므로 선언만 한다.
namespace ip = boost::asio::ip;

class bridge : public boost::enable_shared_from_this<bridge>  // 상속. 상속을 받는 방법은  class 파생클래스이름 : 접근제어지시자 부모클래스이름
                                                              // 즉, class bridge에서 public boost::enable_shared_from_this<bridge>의 것들을 쓸 수 있다.
{
   public:
    // typedef는 멤버변수 선언이 아니다.
    typedef ip::tcp::socket socket_type;         // typedef는 타입의 별칭 생성. ip::tcp::socket의 별칭이 socket_type
    typedef boost::shared_ptr<bridge> ptr_type;  // ptr_type은 타입의 별칭. shared_ptr은 스마트 포인터

    bridge(boost::asio::io_service &ios);  // 생성자

    socket_type &downstream_socket();  // &를 붙이면 주소(참조)가 된다.
    socket_type &upstream_socket();

    // start 함수
    void start(const std::string &upstream_host, unsigned short upstream_port);

    // handle_upstream_connect 함수
    void handle_upstream_connect(const boost::system::error_code &error);

   private:
    /*
       Section A: Remote Server --> Proxy --> Client (다운스트림)
       Process data recieved from remote sever then send to client. (서버로부터 데이터를 받아서 클라이언트에 보낸다)
    */

    // Read from remote server complete, now send data to client (서버로부터 데이터를 읽어옴)
    void handle_upstream_read(const boost::system::error_code &error, const size_t &bytes_transferred);

    // Write to client complete, Async read from remote server (클라이언트에 데이터를 쓴다 = 보낸다)
    void handle_downstream_write(const boost::system::error_code &error);

    // *** End Of Section A *** (다운스트림 완료)

    /*
       Section B: Client --> Proxy --> Remove Server (업스트림)
       Process data recieved from client then write to remove server. (클라이언트로부터 데이터를 받아 서버에 쓴다)
    */

    // Read from client complete, now send data to remote server (클라이언트로부터 데이터를 읽어옴)
    void handle_downstream_read(const boost::system::error_code &error, const size_t &bytes_transferred);

    // Write to remote server complete, Async read from client (서버에 데이터를 쓴다 = 보낸다)
    void handle_upstream_write(const boost::system::error_code &error);

    // *** End Of Section B *** (업스트림 완료)

    void close();  // 종료함수

    socket_type downstream_socket_;  // 멤버변수는 private에 선언. (downstream_socket_, upstream_socket_ 2개의 멤버변수 생성)
    socket_type upstream_socket_;

    enum {
        max_data_length = 8192
    };                                                // 8KB
    unsigned char downstream_data_[max_data_length];  // 멤버변수
    unsigned char upstream_data_[max_data_length];

    boost::mutex mutex_;
};
}  // namespace yhbae
