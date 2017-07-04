#include "Arduino.h"
#include "WebServerTask.h"
#include "ESP8266WebServer.h"
#include "utils.h"

ESP8266WebServer webServer(80);

void WebServerTask::registerBuffersData(DataBufferManager& data)
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
    webPage += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>\n";
//    webPage += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js\"></script>\n";
    webPage += "<script src=\"https://cdn.plot.ly/plotly-latest.min.js\"></script>";


    webPage += "</head>\n<body>\n <div style=\"max-width: 650px;\"> \n";
    //content
    webPage += "<h1>MeteoStation v1.0</h1>";

    DataBufferManager* data = getBuffer();
    webPage += "<div id=\"tempPresure\" style=\"width:100%;height:500px;\"></div>";

    webPage += "(c) 2017 HomeMeteo by arek gorzawski";
    webPage += "</body>\n";

    // JS for the plot
    webPage += "<script>TEMPPRESSURE=document.getElementById('tempPresure'); \n";
    webPage += " var layout = {yaxis:{title:'[&deg;C]'}, yaxis2: { title:'[hPa]',overlaying:'y', side:'right'}}; \n";
    String varData = "var data = [";
    for (int id = 0; id < 2; id++)
    {
      varData += getChannelName(id) + ",";
      webPage += getJavaScriptEntriesForAChannel(id, id);
    }
    varData += "];\n";
    webPage += varData;
    webPage += "Plotly.plot(TEMPPRESSURE, data, layout); </script>\n";

    webPage += "</html> \n";
    return webPage;
}

String WebServerTask::getJavaScriptEntriesForAChannel(uint32_t id, uint32_t axis)
{
    DataBufferManager* data = getBuffer();

    float* dataArray = data -> getDataAll(id, DataBufferManager::BufferLength::MINUTE);
    float* dataArray1h = data -> getDataAll(id, DataBufferManager::BufferLength::HOUR);
    float* dataArray60h = data -> getDataAll(id, DataBufferManager::BufferLength::SIXTY_HOURS);

    String toReturn = "var " + getChannelName(id) + "={";
    String x = "x:[";
    String y = "y:[";
    int counter = 0;
    for (int i = 0; i <  data -> getUsed(id, DataBufferManager::BufferLength::SIXTY_HOURS); i++)
    {
      if (dataArray60h[i] == infinityf()) continue;
      x +=  String(counter++) + ",";
      y +=  String(dataArray60h[i])+ ",";
    }

    for (int i = 0; i <  data -> getUsed(id, DataBufferManager::BufferLength::HOUR); i++)
    {
      if (dataArray1h[i] == infinityf()) continue;
      x +=  String(counter++) + "," ;
      y +=  String(dataArray1h[i]) + ",";

    }

    size_t minuteUsed = data -> getUsed(id, DataBufferManager::BufferLength::MINUTE);
    for (int i = 0; i <  minuteUsed; i++)
    {
      if (dataArray[i] == infinityf()) continue;
      if (i ==  minuteUsed - 1 )
      {
        x +=  String(counter++);
        y +=  String(dataArray[i]);
      }
      else
      {
        x +=   String(counter++) + ",";
        y +=  String(dataArray[i]) + ",";
      }
    }

    x += "]";
    y += "]";

    toReturn += x +"," + y +", type: 'scatter'";
    if (axis == 1)
      toReturn += ", yaxis: 'y2'";
    toReturn += "};";
    return toReturn;
}

String WebServerTask::getChannelName(uint32_t id)
{
  return "data"+String(id);
}

DataBufferManager* WebServerTask::getBuffer()
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
