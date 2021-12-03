#pragma once // 헤더가드
#include "common.hpp"

namespace yhbae
{ // 헤더파일이므로 선언만 한다.
   namespace ip = boost::asio::ip;

   class bridge : public boost::enable_shared_from_this<bridge> // 상속. 상속을 받는 방법은  class 파생클래스이름 : 접근제어지시자 부모클래스이름
                                                                // 즉, 여기선 class bridge에서 public boost::enable_shared_from_this<bridge>의 것들을 쓸 수 있다.
   {
   public:
      typedef ip::tcp::socket socket_type;        // typedef는 타입의 별칭 생성. ip::tcp::socket의 별칭이 socket_type
      typedef boost::shared_ptr<bridge> ptr_type; // ptr_type은 타입의 별칭. shared_ptr은 스마트 포인터

      bridge(boost::asio::io_service &ios); // 생성자

      socket_type &downstream_socket() // &를 붙이면 주소가 된다. 멤버함수(Method), socket_type은 return값의 타입이다.
      {
         // Client socket
         return downstream_socket_;
      }

      socket_type &upstream_socket()
      {
         // Remote server socket
         return upstream_socket_;
      }

      // start 함수
      void start(const std::string &upstream_host, unsigned short upstream_port) // void는 return이 없음.
      {
         // Attempt connection to remote server (upstream side)
         upstream_socket_.async_connect( // async_connect는 소켓을 지정된 원격 엔드포인트로 비동기 연결하는데 사용 (비동기 연결을 시작한다.)
             ip::tcp::endpoint(
                 boost::asio::ip::address::from_string(upstream_host), // 파라미터에 입력된 host와 port에 연결
                 upstream_port),
             boost::bind(&bridge::handle_upstream_connect, // boost::bind() 에게 첫번째로 들어갈 인자는 바인딩할 함수의 주소
                                                           // handle_upstream_connect함수를 start에 바인딩?
                         shared_from_this(),               // 그 다음의 인자들 바인딩될시 매핑될 매개 변수 리스트들
                         boost::asio::placeholders::error));
      }

      // handle_upstream_connect 함수
      void handle_upstream_connect(const boost::system::error_code &error)
      {
         if (!error)
         {
            // Setup async read from remote server (upstream)
            upstream_socket_.async_read_some(
                boost::asio::buffer(upstream_data_, max_data_length),
                boost::bind(&bridge::handle_upstream_read,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            // Setup async read from client (downstream)
            downstream_socket_.async_read_some(
                boost::asio::buffer(downstream_data_, max_data_length),
                boost::bind(&bridge::handle_downstream_read,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
         }
         else
            close(); // error시 종료
      }

   private:
      /*
         Section A: Remote Server --> Proxy --> Client (다운스트림)
         Process data recieved from remote sever then send to client. (서버로부터 데이터를 받아서 클라이언트에 보낸다)
      */

      // Read from remote server complete, now send data to client (서버로부터 데이터를 읽어옴)
      void handle_upstream_read(const boost::system::error_code &error, const size_t &bytes_transferred)
      {
         if (!error)
         {
            async_write(downstream_socket_,
                        boost::asio::buffer(upstream_data_, bytes_transferred),
                        boost::bind(&bridge::handle_downstream_write,
                                    shared_from_this(),
                                    boost::asio::placeholders::error));
         }
         else
            close();
      }

      // Write to client complete, Async read from remote server (클라이언트에 데이터를 쓴다 = 보낸다)
      void handle_downstream_write(const boost::system::error_code &error)
      {
         if (!error)
         {
            upstream_socket_.async_read_some(
                boost::asio::buffer(upstream_data_, max_data_length),
                boost::bind(&bridge::handle_upstream_read,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
         }
         else
            close();
      }
      // *** End Of Section A *** (다운스트림 완료)

      /*
         Section B: Client --> Proxy --> Remove Server (업스트림)
         Process data recieved from client then write to remove server. (클라이언트로부터 데이터를 받아 서버에 쓴다)
      */

      // Read from client complete, now send data to remote server (클라이언트로부터 데이터를 읽어옴)
      void handle_downstream_read(const boost::system::error_code &error, const size_t &bytes_transferred)
      {
         if (!error)
         {
            async_write(upstream_socket_,
                        boost::asio::buffer(downstream_data_, bytes_transferred),
                        boost::bind(&bridge::handle_upstream_write,
                                    shared_from_this(),
                                    boost::asio::placeholders::error));
         }
         else
            close();
      }

      // Write to remote server complete, Async read from client (서버에 데이터를 쓴다 = 보낸다)
      void handle_upstream_write(const boost::system::error_code &error)
      {
         if (!error)
         {
            downstream_socket_.async_read_some(
                boost::asio::buffer(downstream_data_, max_data_length),
                boost::bind(&bridge::handle_downstream_read,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
         }
         else
            close();
      }
      // *** End Of Section B *** (업스트림 완료)

      void close() // 종료함수
      {
         boost::mutex::scoped_lock lock(mutex_);

         if (downstream_socket_.is_open()) // 소켓이 만약 열려있으면 닫는다.
         {
            downstream_socket_.close();
         }

         if (upstream_socket_.is_open())
         {
            upstream_socket_.close();
         }
      }

      socket_type downstream_socket_; // 멤버변수는 private에 선언. (downstream_socket_, upstream_socket_ 2개의 멤버변수 생성)
      socket_type upstream_socket_;

      enum
      {
         max_data_length = 8192
      };                                               // 8KB
      unsigned char downstream_data_[max_data_length]; // 멤버변수
      unsigned char upstream_data_[max_data_length];

      boost::mutex mutex_;
   };
}
