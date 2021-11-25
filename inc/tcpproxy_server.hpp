#pragma once
#include "common.hpp"

namespace yhbae
{
   namespace ip = boost::asio::ip;

   class bridge : public boost::enable_shared_from_this<bridge>
   {
   public:
      typedef ip::tcp::socket socket_type;        // typedef는 타입의 별칭 생성. ip::tcp::socket의 별칭이 socket_type
      typedef boost::shared_ptr<bridge> ptr_type; // ptr_type은 타입의 별칭. shared_ptr은 스마트 포인터

      bridge(boost::asio::io_service &ios) // 생성자
          : downstream_socket_(ios),       // 멤버변수 down, upstream_socket에 파라미터를 연결해줌.
            upstream_socket_(ios)
      {
      }

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
         upstream_socket_.async_connect( // async_connect는 소켓을 지정된 원격 엔드포인트로 비동기 연결하는데 사용
             ip::tcp::endpoint(
                 boost::asio::ip::address::from_string(upstream_host), // 파라미터에 입력된 host와 port에 연결
                 upstream_port),
             boost::bind(&bridge::handle_upstream_connect,
                         shared_from_this(),
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
         Process data recieved from remote sever then send to client.
      */

      // Read from remote server complete, now send data to client
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

      // Write to client complete, Async read from remote server
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
      // *** End Of Section A ***

      /*
         Section B: Client --> Proxy --> Remove Server (업스트림) (다운스트림 read & 업스트림 write)
         Process data recieved from client then write to remove server.
      */

      // Read from client complete, now send data to remote server
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

      // Write to remote server complete, Async read from client
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
      // *** End Of Section B ***

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

      socket_type downstream_socket_; // 멤버변수는 private에 선언. (down, upstream_socket 2개의 멤버변수 생성)
      socket_type upstream_socket_;

      enum
      {
         max_data_length = 8192
      }; //8KB
      unsigned char downstream_data_[max_data_length];
      unsigned char upstream_data_[max_data_length];

      boost::mutex mutex_;

   public:
      class acceptor // bridge 클래스 안에 acceptor 클래스 생성!
      {
      public:
         acceptor(boost::asio::io_service &io_service, // 생성자 (만드려면 인자가 5개 필요하다)
                  const std::string &local_host, unsigned short local_port,
                  const std::string &upstream_host, unsigned short upstream_port)
             : io_service_(io_service),
               localhost_address(boost::asio::ip::address_v4::from_string(local_host)),
               acceptor_(io_service_, ip::tcp::endpoint(localhost_address, local_port)),
               upstream_port_(upstream_port),
               upstream_host_(upstream_host)
         {
         }

         // accept_connections 함수
         bool accept_connections()
         {
            try
            {
               session_ = boost::shared_ptr<bridge>(new bridge(io_service_)); // 동적할당

               acceptor_.async_accept(session_->downstream_socket(), // downstream_socket이란 method 호출
                                      boost::bind(&acceptor::handle_accept,
                                                  this,
                                                  boost::asio::placeholders::error));
            }
            catch (std::exception &e)
            {
               std::cerr << "acceptor exception: " << e.what() << std::endl;
               return false;
            }

            return true;
         }

      private:
         // handle_accept 함수
         void handle_accept(const boost::system::error_code &error)
         {
            if (!error)
            {
               session_->start(upstream_host_, upstream_port_); // start 함수 호출

               if (!accept_connections()) // accept_connections이 false면
               {
                  std::cerr << "Failure during call to accept." << std::endl;
               }
            }
            else
            {
               std::cerr << "Error: " << error.message() << std::endl;
            }
         }

         boost::asio::io_service &io_service_; // 멤버변수 선언 (in Private)
         ip::address_v4 localhost_address;
         ip::tcp::acceptor acceptor_;
         ptr_type session_;
         unsigned short upstream_port_;
         std::string upstream_host_;
      };
   };
}