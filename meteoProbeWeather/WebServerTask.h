
#ifndef WEBSERVERTASK_H_
#define WEBSERVERTASK_H_

#include "task.hpp"
#include "Arduino.h"
#include "DataBufferManager.h"

extern String webMessage;

class WebServerTask: public Tasks::Task {
public:
  WebServerTask();

  virtual void run();
  virtual void reset();
  virtual ~WebServerTask();
  bool started = false;
  void registerBuffersData(DataBufferManager& dataBufferManager);

private:
  uint64_t cntr = 0;
  DataBufferManager* _data = NULL;
  DataBufferManager* getBuffer();
  String updateWebPage();
};

#endif /* WEBSERVERTASK_H_ */
