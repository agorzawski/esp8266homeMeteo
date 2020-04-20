#include "Arduino.h"
#include "WebServerTask.h"
#include "ESP8266WebServer.h"
#include "MacroStringReplace.h"
#include "utils.h"

ESP8266WebServer webServer(80);

void WebServerTask::registerBuffersData(DataBufferManager& data)
{
    _data = &data;
}

void WebServerTask::setFirmwareVersion(String ver)
{
  _ver = ver;
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
    String title ="<h1>MeteoStation $VER</h1>";
    title.replace("$VER", _ver);
    webPage += title;
    webPage += "<p><a href=\"/config\">CONFIG </a></p>";

    //DataBufferManager* data = getBuffer();
    //  webPage += "<div id=\"tempPresure\" style=\"width:100%;height:500px;\"></div>";

    webPage += "(c) 2017-2020 HomeMeteo by arek gorzawski";
    webPage += "</body>\n";

    // JS for the plot
    // webPage += "<script>TEMPPRESSURE=document.getElementById('tempPresure'); \n";
    // webPage += " var layout = {yaxis:{title:'[&deg;C]'}, yaxis2: { title:'[hPa]',overlaying:'y', side:'right'}}; \n";
    // String varData = "var data = [";
    // for (int id = 0; id < 2; id++)
    // {
    //   varData += getChannelName(id) + ",";
    //   webPage += getJavaScriptEntriesForAChannel(id, id);
    // }
    // varData += "];\n";
    // webPage += varData;
    // webPage += "Plotly.plot(TEMPPRESSURE, data, layout); </script>\n";

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
    int hours = 0;
    int minutes = 0;

    int counter = 0;
    // for (int i = 0; i <  data -> getUsed(id, DataBufferManager::BufferLength::SIXTY_HOURS); i++)
    // {
    //   if (dataArray60h[i] == infinityf() || dataArray60h[i] - 1.99 < 0.1) continue;
    //   hours = (i + 1) * 3600;
    //   x +=  String(hours) + ",";
    //   y +=  String(dataArray60h[i])+ ",";
    //   counter++;
    // }
    //
    // size_t minutesUsedInHour = data -> getUsed(id, DataBufferManager::BufferLength::HOUR);
    // for (int i = 0; i <  minutesUsedInHour; i++)
    // {
    //   if (dataArray1h[i] == infinityf() || dataArray1h[i] - 1.99 < 0.1 ) continue;
    //   minutes = hours + (i + 1) * 60;
    //
    //   if (i ==  minutesUsedInHour - 1 )
    //   {
    //     x +=  String(minutes);
    //     y +=  String(dataArray1h[i]);
    //   }
    //   else
    //   {
    //     x +=  String(minutes) + ",";
    //     y +=  String(dataArray1h[i]) + ",";
    //   }
    //   counter++;
    // }

    // size_t minuteUsed = data -> getUsed(id, DataBufferManager::BufferLength::MINUTE);
    // for (int i = 0; i <  minuteUsed; i++)
    // {
    //   if (dataArray[i] == infinityf()) continue;
    //   if (i ==  minuteUsed - 1 )
    //   {
    //     x +=  String(minutes + counter++);
    //     y +=  String(dataArray[i]);
    //   }
    //   else
    //   {
    //     x +=   String(minutes + counter++) + ",";
    //     y +=  String(dataArray[i]) + ",";
    //   }
    // }

    x += "]";
    y += "]";

    toReturn += x +"," + y +", type: 'scatter'";
    if (axis == 1)
      toReturn += ", yaxis: 'y2'";
    toReturn += "};";
    return toReturn;
}

static const char configPage[] PROGMEM = R"_(
  <html><head>
  <meta charset="utf-8" name="viewport" content="width=device-width, initial-scale=1">
  <title>$title$</title><style>
    h2 {color: green;}
    th {font-weight: bold; font-size: 130%;}
    .l {text-align: right; font-style: italic;}
    a {text-decoration: none;}
  </style></head>
  <body>
  <a href="/">&lArr;	</a>
   <form action="/config" method="POST">
   <table>
      <tr><th>Config</th><th width="50%"/><th/></tr>
      <tr><td colspan="3"><textarea cols="60" rows="40" autofocus="true" name="content">$configFileContents$</textarea></td></tr>
      <tr><td/><td/><td><input type="submit" value="Save"></td></tr>
    </table>
    </form>
  </body>
</html>
)_";

FlashStream configPageFS(configPage);

void WebServerTask::handleConfig()
{
	//if (!handleAuth(webServer))
	//	return;

	String content;

	if (webServer.method() == HTTP_GET)
	{
		//read config from the file
		SPIFFS.begin();
		auto file = SPIFFS.open("/config.txt", "r");
		if (!file)
			content = F("# The file is empty, please create a new one!");
		else
		{
			content = file.readString();
			file.close();
		}
		SPIFFS.end();
	}
	else
	{
		//POST
		//load content from variable
		content = webServer.arg(F("content"));
		SPIFFS.begin();
		auto file = SPIFFS.open("/config.txt", "w+");
		file.print(content);
		file.close();
		SPIFFS.end();
		readConfigFromFS();
	}

	StringStream ss(2048);
	macroStringReplace(configPageFS, constString(content), ss);
	webServer.send(200, "text/html", ss.buffer);
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
		webServer.on("/config", [this]{handleConfig();});
    webServer.begin();

    logPrintf("WebServerTask - ready!");
    return;
  }
  webServer.handleClient();
}

WebServerTask::~WebServerTask()
{
}
