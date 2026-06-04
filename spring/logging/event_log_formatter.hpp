#pragma once

namespace euclid {
namespace spring {

template <typename EventLog>
class EventLogFormatter {
public:  
  explicit EventLogFormatter() = default;
  ~EventLogFormatter() = default;
  
  EventLogFormatter(EventLogFormatter&) = delete;
  EventLogFormatter& operator=(EventLogFormatter&) = delete;
  EventLogFormatter(EventLogFormatter&&) = delete;
  EventLogFormatter& operator=(EventLogFormatter&&) = delete;

  inline void format_event_log(const EventLog& event_log, std::string& event_log_str) const {
    event_log_str += (
      "log_ts_ns = " + std::to_string(event_log.log_ts_ns) +
      " seq_no = " + std::to_string(event_log.seq_no) + 
      " producer_id = " + std::to_string(event_log.producer_id) + 
      " log_stage = " + to_string(event_log.event_stage) + 
      "\n"
    );
  }

  inline const char* to_string(const EventStage event_stage) const {
    switch (event_stage) {
      case(EventStage::MarketGenerated):
        return "MarketGenerated";
      default:
        return "NIL_STAGE";
    }
  }
};

} // namespace spring
} // namespace euclid