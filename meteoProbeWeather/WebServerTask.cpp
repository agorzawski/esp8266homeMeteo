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
    webPage += "<link rel=\"stylesheet\" href=\"http://agorzaws.web.cern.ch/agorzaws/thermometer.css\">\n";

    webPage += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js\"></script>\n";
    webPage += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>\n";
    //webPage += "<style></style>";

    webPage += "</head>\n<body>\n <div style=\"max-width: 600px;\"> \n";
    //content
    webPage += "<h1>MeteoStation v0.2</h1>";

    BufferedMeteoData* data = getBuffer();

    webPage += "<hr>";
    webPage += "<h2>Last data:</h2>";
    float lastTemp = data -> getData(0);
    float lastPressure = data -> getData(1);

    webPage += "<h4>" + String(lastTemp) + "&deg;C,  " + String(lastPressure)+ "mBar</h4>";
    webPage += "<hr>";

    webPage += "<div>";
    webPage += "<div class=\"leftdisp\"><span class=\"thermometer\">"+ String(lastTemp) + "&deg;C</span></div>";
    webPage += "<div class=\"rightdisp\"><span class=\"thermometer\">"+ String(lastPressure)+ "mBar</span></div>";
    webPage += "</div>";
//    webPage += "<hr>";
//    webPage += "<h4>Temperature</h4>";
    float* tempData =  data -> getDataAll(0);
    float* pressureData =  data -> getDataAll(1);
    for (int i=0; i < 4; i++)
    {
      webPage += String(tempData[i]) + "degC, " + String(pressureData[i]) + " mBar<br> ";
    }
    webPage += "2017 arek gorzawski";
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

    webServer.on("/", [this](){
      webServer.send(200, "text/html", updateWebPage() );
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
