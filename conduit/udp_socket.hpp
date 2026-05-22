#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <type_traits>
#include <unistd.h>

#include "conduit/udp_socket_cfg.hpp"

namespace euclid {
namespace conduit {

template <typename Packet>
class UdpSocket {
 static_assert(std::is_trivially_copyable_v<Packet>);
 static_assert(std::is_standard_layout_v<Packet>);

 public:
  explicit UdpSocket(UdpSocketCfg cfg) : cfg_(cfg) {}
  ~UdpSocket() {
    if (fd_ >= 0) {
      ::close(fd_);
    }
  }

  UdpSocket(const UdpSocket&) = delete;
  UdpSocket& operator=(const UdpSocket&) = delete;
  UdpSocket(UdpSocket&&) = delete;
  UdpSocket& operator=(UdpSocket&&) = delete;

  bool open() {
    if (!populate_addresses()) return false;
    if (!open_socket()) return false;
    if (!bind_socket()) return false;
    if (!join_group()) return false;
    return true;
  }

  bool send_packet(const Packet& pkt) {
    const ssize_t n = ::sendto(
      fd_,
      &pkt,
      sizeof(Packet),
      0,
      reinterpret_cast<const sockaddr*>(&dest_addr_),
      sizeof(dest_addr_)
    );

    return n == static_cast<ssize_t>(sizeof(Packet));
  }

  bool recv_packet(Packet& pkt) {
    const ssize_t n = ::recvfrom(
      fd_,
      &pkt,
      sizeof(Packet),
      0,
      nullptr,
      nullptr
    );
    
    return n == static_cast<ssize_t>(sizeof(Packet));
  }

 private:
  UdpSocketCfg cfg_;

  int fd_ = -1;

  sockaddr_in local_addr_{};
  sockaddr_in dest_addr_{};
  ip_mreq mreq_{};

 private:
  bool populate_addresses() noexcept {
    // used to ::bind
    local_addr_.sin_family = AF_INET;
    local_addr_.sin_port = htons(cfg_.port);
    if (::inet_pton(AF_INET, cfg_.bind_ip, &local_addr_.sin_addr) != 1) {
      return false;
    }
    
    // used to send_packet()
    dest_addr_.sin_family = AF_INET;
    dest_addr_.sin_port = htons(cfg_.port);
    if (::inet_pton(AF_INET, cfg_.group_ip, &dest_addr_.sin_addr) != 1) {
      return false;
    }

    // local interface ip
    if (::inet_pton(AF_INET, cfg_.bind_ip, &mreq_.imr_interface) != 1) {
      return false;
    }
    // multicast group membership
    if (::inet_pton(AF_INET, cfg_.group_ip, &mreq_.imr_multiaddr) != 1) {
      return false;
    }

    return true;
  }

  bool open_socket() noexcept {
    fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    return (fd_ >= 0);
  }

  bool bind_socket() noexcept {
    int rc = ::bind(
      fd_, 
      reinterpret_cast<const sockaddr*>(&local_addr_), 
      sizeof(local_addr_)
    );

    return rc == 0;
  }

  bool join_group() noexcept {
    int rc = ::setsockopt(
        fd_,
        IPPROTO_IP,
        IP_ADD_MEMBERSHIP,
        &mreq_,
        sizeof(mreq_)
    );

    return rc == 0;
  }
};

} // namespace conduit
} // namespace euclid