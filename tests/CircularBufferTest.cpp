#include "../meteoProbeWeather/CircularBuffer.h"
#include <iostream>

using namespace std;
int main()
{
  CircularBuffer<float, 300> dataBuffer;

  for (int i=0; i< 21; i++)
  {
      float data = i * 0.1;
      dataBuffer.write( &data , 1);
      cout << "should be " << data << ", is: " << dataBuffer.read() << "\n";

      int buffer = 10;

      if (i  == 6  || i == 17)
      {
        float temp[buffer];
        dataBuffer.read(temp, buffer);

        cout << " Last buffer " << buffer << " read: ";
        for (int j = 0; j < buffer; j++)
        {
          cout << temp[j] << ", ";
        }
        cout << "\n";
      }
  }

  return 0;
}
