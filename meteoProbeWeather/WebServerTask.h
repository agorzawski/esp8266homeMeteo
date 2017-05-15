
#ifndef WEBSERVERTASK_H_
#define WEBSERVERTASK_H_

#include "task.hpp"
#include "Arduino.h"
#include "BufferedMeteoData.h"

extern String webMessage;

class WebServerTask: public Tasks::Task {
public:
  WebServerTask();

  virtual void run();
  virtual void reset();
  virtual ~WebServerTask();
  bool started = false;
  void registerBuffersData(BufferedMeteoData& data);

private:
  uint64_t cntr = 0;
  BufferedMeteoData* _data = NULL;
  BufferedMeteoData* getBuffer();
  String updateWebPage(); 
};

#endif /* WEBSERVERTASK_H_ */
