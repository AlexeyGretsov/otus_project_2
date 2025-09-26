#include "net/session_manager.h"

void SessionsManager::addSession(MessagesProcessorPtr processor) {
  processors.insert(processor);
}
void SessionsManager::removeSession(MessagesProcessorPtr processor) {
  processors.erase(processor);
}
void SessionsManager::checkProcessedMessages() {
  for (auto processor : processors) {
    processor->checkProcessedMessages();
  }
}
