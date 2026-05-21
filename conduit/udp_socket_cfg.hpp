#pragma once

#include <cstdint>
#include <string_view>

namespace euclid {
namespace conduit {

struct UdpSocketCfg {
  std::string_view group_ip;
  std::string_view bind_ip;
  std::uint16_t port;
};

} // namespace conduit
} // namespace euclid