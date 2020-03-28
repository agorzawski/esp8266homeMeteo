/*
 * utils.cpp
 *
 *  Created on: 04.01.2017
 *      Author: Bartosz Bielawski
 *  Update Feb 2018
 *      Author: Arek Gorzawski
 *  Update March 2020
 *      Author: Bartosz Bielawski
 *      Author: Arek Gorzawski
 */
#include <time.h>
#include <stdio.h>
#include <FS.h>

#include <vector>
#include <memory>
#include <utility>

#include "utils.h"
#include "config.h"
#include "Client.h"
#include "Arduino.h"
//#include "FS.h"

#include "DataStore.h"

extern "C" {
#include "user_interface.h"
}

int operator"" _s(long double seconds) {return seconds * 1000 / MS_PER_CYCLE;}
int operator"" _s(unsigned long long int seconds) {return seconds * 1000 / MS_PER_CYCLE;}


//static char dateTimeBuffer[] = "00/00/00 00:00:00";
static char dateTimeBuffer[] = "[...]";

static time_t previousDateTime = 0;

void blinkStatus(int ledId, int high, int low)
{
  digitalWrite(ledId, HIGH);
  delay(high);
  digitalWrite(ledId, LOW);
  delay(low);
}

const char* getDateTime()
{
  time_t now = time(nullptr);
  if (now == previousDateTime)
    return dateTimeBuffer;

  auto lt = localtime(&now);
  snprintf(dateTimeBuffer, 32, "%02d/%02d/%02d %02d:%02d:%02d",
      lt->tm_year-100,
      lt->tm_mon+1,
      lt->tm_mday,
      lt->tm_hour,
      lt->tm_min,
      lt->tm_sec);

  previousDateTime = now;
  return dateTimeBuffer;
}

const static char UUID_FORMAT[] PROGMEM = "%08x-%04x-4%03x-8%03x-%04x%08x";
static char UUID[36];

const char* generateRandomUUID()
{
  uint32_t r1 = os_random();
  uint32_t r2 = os_random();
  uint32_t r3 = os_random();
  uint32_t r4 = os_random();

  sprintf_P(UUID, UUID_FORMAT, r1, r2 >> 16, r2 & 0xFFF, r3 >> 20, r3 & 0xFFFF, r4);
  return UUID;
}

void sendWSPacket_P(uint8_t header, uint16_t size, const uint8_t* key, PGM_P payload, Client* client)
{
  client->write(header);

  if (size >= 0x7E)
  {
    client->write(0xFE);
    client->write(size >> 8);
    client->write(size & 0xFF);
  }
  else
  {
    client->write((size & 0x7F) | 0x80);
  }

  client->write(key, 4);

  for (int i = 0; i < size; i++)
  {
    client->write(pgm_read_byte(payload+i) ^ key[i % 4]);
  }
}

void sendWSPacket(uint8_t header, uint16_t size, const uint8_t* key, const char* payload, Client* client)
{
  client->write(header);

  if (size >= 0x7E)
  {
    client->write(0xFE);
    client->write(size >> 8);
    client->write(size & 0xFF);
  }
  else
  {
    client->write((size & 0x7F) | 0x80);
  }

  client->write(key, 4);

  for (int i = 0; i < size; i++)
  {
    client->write(payload[i] ^ key[i % 4]);
  }
}

char* toCharArray(const char* format, ...)
{
  char* localBuffer = new char[96];
  va_list argList;
  va_start(argList, format);
  vsnprintf(localBuffer, sizeof(localBuffer), format, argList);
  va_end(argList);
  //Serial.printf("Prepared - ");
  Serial.println(localBuffer);
  return localBuffer;
}

void logPrintf(char* format, ...)
{
  char localBuffer[128];
  va_list argList;
  va_start(argList, format);
  Serial.printf("%s - ", getDateTime());
  vsnprintf(localBuffer, sizeof(localBuffer), format, argList);
  Serial.println(localBuffer);
  va_end(argList);
}

void logPrintf(const __FlashStringHelper* format, ...)
{
  char localBuffer[128];
  va_list argList;
  va_start(argList, format);
  Serial.printf("%s - ", getDateTime());
  vsnprintf_P(localBuffer, sizeof(localBuffer), (PGM_P)format, argList);
  Serial.println(localBuffer);
  va_end(argList);
}

void logPrintfX(const String& app, const String& format, ...)
{
	char localBuffer[256];
	String a(app);
	va_list argList;
	va_start(argList, format);
	uint32_t bytes = snprintf(localBuffer, sizeof(localBuffer), "%s - %s: ", getDateTime(), a.c_str());
	vsnprintf(localBuffer+bytes, sizeof(localBuffer)-bytes, format.c_str(), argList);
	Serial.println(localBuffer);

	//syslogSend(app, localBuffer+bytes);

	va_end(argList);
}

bool checkFileSystem()
{
  bool alreadyFormatted = SPIFFS.begin();
  if (not alreadyFormatted)
    SPIFFS.format();
  SPIFFS.end();
  return alreadyFormatted;
}

String readLine(fs::File& file)
{
	String result;

	while (file.available())
	{
		int c = file.read();
		if (c == '\n')
			return result;

		if (c == '\r')
			return result;

		//cast it, otherwise a number is appended - not a char
		result += (char)c;
	}

	return result;
}

std::pair<String, String> splitLine(String&& line)
{
    std::pair<String, String> result;

    line.trim();

    if (line.length() == 0)
        return result;

    if (line[0] == '#')
        return result;

    auto pos = line.indexOf('=');   //find the first character

    if (pos == -1)
    {
        result.first = line;
        return result;
    }

    result.first = line.substring(0, pos);
    line.remove(0, pos+1);          //remove the equal sign as well
    result.second = line;
    return result;
}


String readConfig(const String& name)
{
  auto f = SPIFFS.open(String(F("/config/")) +name, "r");
  auto result = f.readStringUntil('\n');
  return result;
}

void writeConfig(const String& name, const String& value)
{
  auto f = SPIFFS.open(String(F("/config/")) +name, "w+");
  f.print(value);
  f.print('\n');
}


void readConfigFromFS()
{
    logPrintfX("UTL", F("Reading configuration values from the flash..."));
    //the FS has to be initialized already...
	SPIFFS.begin();
    auto file = SPIFFS.open("/config.txt", "r");
    if (!file)
	{
		logPrintfX(F("UTL"), F("The file is missing, please create your own config using the web interface!"));
		return;
	}

	logPrintfX(F("UTL"), "File size: %zu", file.size());

	//remove all the data that's already present
	DataStore::clear();

    while (file.available())
    {
	    auto p = splitLine(readLine(file));
        if (not p.second.length())
            continue;

        logPrintfX("UTL", F("Config: %s = %s"), p.first.c_str(), p.second.c_str());
		DataStore::value(p.first) = p.second;
    }
	SPIFFS.end();
}


void saveToRTC(uint32_t address, uint32_t data)
{
  //  system_rtc_mem_write((address >> 2) + 64, &data, 4);
}

uint32_t readFromRTC(uint32_t address)
{
  //  uint32_t result = 0;
  //  system_rtc_mem_read((address >> 2) + 64, &data, 4);
  //  return result;
}

int32_t getTimeZone()
{
  return readConfig("timezone").toInt();
}
