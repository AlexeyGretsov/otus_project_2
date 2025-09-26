#pragma once

class MessagesProcessor {
public:
  virtual ~MessagesProcessor() {}

  virtual void checkProcessedMessages() = 0;
};
