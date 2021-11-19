// #pragma once
// // #include "../inc/common.hpp"
// #include "../inc/tcpproxy_server.hpp"
// // #include "common.hpp"


// namespace yhbae
// {
//    bridge(boost::asio::io_service& ios): downstream_socket_(ios),
//          upstream_socket_(ios)
//    {
//    }

//    socket_type &downstream_socket()
//    {
//       // Client socket
//       return downstream_socket_;
//    }

//    socket_type &upstream_socket()
//    {
//       // Remote server socket
//       return upstream_socket_;
//    }

//    void start(const std::string &upstream_host, unsigned short upstream_port)
//    {
//       // Attempt connection to remote server (upstream side)
//       upstream_socket_.async_connect(
//           ip::tcp::endpoint(
//               boost::asio::ip::address::from_string(upstream_host),
//               upstream_port),
//           boost::bind(&bridge::handle_upstream_connect,
//                       shared_from_this(),
//                       boost::asio::placeholders::error));
//    }

//    void handle_upstream_connect(const boost::system::error_code &error)
//    {
//       if (!error)
//       {
//          // Setup async read from remote server (upstream)
//          upstream_socket_.async_read_some(
//              boost::asio::buffer(upstream_data_, max_data_length),
//              boost::bind(&bridge::handle_upstream_read,
//                          shared_from_this(),
//                          boost::asio::placeholders::error,
//                          boost::asio::placeholders::bytes_transferred));

//          // Setup async read from client (downstream)
//          downstream_socket_.async_read_some(
//              boost::asio::buffer(downstream_data_, max_data_length),
//              boost::bind(&bridge::handle_downstream_read,
//                          shared_from_this(),
//                          boost::asio::placeholders::error,
//                          boost::asio::placeholders::bytes_transferred));
//       }
//       else
//          close();
//    }

//    void handle_upstream_read(const boost::system::error_code &error,
//                              const size_t &bytes_transferred)
//    {
//       if (!error)
//       {
//          async_write(downstream_socket_,
//                      boost::asio::buffer(upstream_data_, bytes_transferred),
//                      boost::bind(&bridge::handle_downstream_write,
//                                  shared_from_this(),
//                                  boost::asio::placeholders::error));
//       }
//       else
//          close();
//    }

//    void handle_downstream_write(const boost::system::error_code &error)
//    {
//       if (!error)
//       {
//          upstream_socket_.async_read_some(
//              boost::asio::buffer(upstream_data_, max_data_length),
//              boost::bind(&bridge::handle_upstream_read,
//                          shared_from_this(),
//                          boost::asio::placeholders::error,
//                          boost::asio::placeholders::bytes_transferred));
//       }
//       else
//          close();
//    }

//    void handle_downstream_read(const boost::system::error_code &error,
//                                const size_t &bytes_transferred)
//    {
//       if (!error)
//       {
//          async_write(upstream_socket_,
//                      boost::asio::buffer(downstream_data_, bytes_transferred),
//                      boost::bind(&bridge::handle_upstream_write,
//                                  shared_from_this(),
//                                  boost::asio::placeholders::error));
//       }
//       else
//          close();
//    }

//    void handle_upstream_write(const boost::system::error_code &error)
//    {
//       if (!error)
//       {
//          downstream_socket_.async_read_some(
//              boost::asio::buffer(downstream_data_, max_data_length),
//              boost::bind(&bridge::handle_downstream_read,
//                          shared_from_this(),
//                          boost::asio::placeholders::error,
//                          boost::asio::placeholders::bytes_transferred));
//       }
//       else
//          close();
//    }

//    void close()
//    {
//       boost::mutex::scoped_lock lock(mutex_);

//       if (downstream_socket_.is_open())
//       {
//          downstream_socket_.close();
//       }

//       if (upstream_socket_.is_open())
//       {
//          upstream_socket_.close();
//       }
//    }

// }
