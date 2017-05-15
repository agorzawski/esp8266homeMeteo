#include "Arduino.h"
#include "WebServerTask.h"
#include "ESP8266WebServer.h"

#include "utils.h"

ESP8266WebServer webServer(80);

void WebServerTask::registerBuffersData(BufferedMeteoData& data)
{
    _data = &data;
}   
 
String WebServerTask::updateWebPage()
{    
    String webPage = "";
    webPage += "<!DOCTYPE HTML>\r\n<html>\n<head>\n";
    webPage += "<meta charset=\"utf-8\"> \n";
    webPage += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
    webPage += "<title>Home Meteo Arduino</title>\n";
    //bootstrap
    webPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">\n";                    
    webPage += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js\"></script>\n";
    webPage += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>\n";      
    webPage += "</head>\n<body>\n <div style=\"max-width: 600px;\"> \n";
    //content
    webPage += "<h1>MeteoStation v0.1</h2>";         

    BufferedMeteoData* data = getBuffer();  
//    float lastTemp = data -> getData(0);
//    float* tempData =  data -> getDataAll(0);
    
//    webPage += String(lastTemp)+" \n ";
    webPage += "</body>\n</html> \n";
    return webPage;
}    

BufferedMeteoData* WebServerTask::getBuffer()
{
  return _data;
}

WebServerTask::WebServerTask()
{
  reset();
}

void WebServerTask::reset()
{
  sleep(5_s);
  webServer.stop();
  sleep(5_s);
  started = false;
}

void WebServerTask::run()
{
  if (!started)
  {
    started = true;
    logPrintf("WebServerTask - configuring server");
    
    webServer.on("/", [](){
      webServer.send(200, "text/html", updateWebPage());
    });

    webServer.begin();

    logPrintf("WebServerTask - ready!");
    return;
  }
  webServer.handleClient();

}

WebServerTask::~WebServerTask()
{
}
