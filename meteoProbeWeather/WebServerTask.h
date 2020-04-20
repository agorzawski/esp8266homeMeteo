
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
  void setFirmwareVersion(String ver);

private:
  uint64_t cntr = 0;
  DataBufferManager* _data = NULL;
  String _ver;
  DataBufferManager* getBuffer();
  String updateWebPage();
  String getJavaScriptEntriesForAChannel(uint32_t id, uint32_t axis);
  String getChannelName(uint32_t id);
  void handleConfig();
};

#endif /* WEBSERVERTASK_H_ */
