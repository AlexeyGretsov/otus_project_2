#pragma once

#include <deque>
#include <memory>
#include <set>

#include "include/i_messages_processor.h"
#include "include/transfer_message.h"

typedef std::deque<TransferMessage> TransferMessageQueue;

using MessagesProcessorPtr = std::shared_ptr<MessagesProcessor>;

class SessionsManager {
public:
  void addSession(MessagesProcessorPtr processor);
  void removeSession(MessagesProcessorPtr processor);
  void checkProcessedMessages();

private:
  std::set<MessagesProcessorPtr> processors;
};
