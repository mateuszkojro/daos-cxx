#ifndef MK_EVENT_QUEUE_H
#define MK_EVENT_QUEUE_H

#include "Errors.h"
#include "Pool.h"
#include "daos.h"
#include "daos_event.h"
#include "daos_types.h"
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <mutex>


#include <iostream>
#include <vector>

class EventQueue {
 public:
  EventQueue(size_t max_inflight);
  EventQueue(EventQueue&&) = delete;
  EventQueue(const EventQueue&) = delete;
  EventQueue& operator=(EventQueue&&) = delete;
  EventQueue& operator=(const EventQueue&) = delete;
  ~EventQueue();

  daos_event_t* get_event();
  void wait();

  std::atomic_size_t waiting_time_ {0};

 private:
  daos_event_t* poll();
  daos_event_t* add_event();
  std::atomic_size_t inflight_ {0};
  const size_t max_inflight_;

  daos_handle_t event_queue_handle_;
  std::vector<daos_event_t> events_;
};
#endif// !MK_EVENT_QUEUE_H
