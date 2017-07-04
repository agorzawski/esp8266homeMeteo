#include "../meteoProbeWeather/CircularBuffer.h"
#include <iostream>

using namespace std;
int main()
{
  CircularBuffer<float, 300> dataBuffer;

  for (int i=0; i< 25; i++)
  {
      float data = i * 0.1;

      //data is in
      dataBuffer.write( &data , 1);

      //let's read that shit
      cout << "step " << i << "; Value should be " << data << ", is: " << dataBuffer.read() << endl;

      // some read in between (some other computation);
      float a = dataBuffer.read();

      int buffer = 10;

      if (i == 6 || i == 17 || i == 90 || i == 201)
      {
        float temp[buffer];
        dataBuffer.read(temp, buffer);

        cout << endl << "Step count " << i << " asking for last " << buffer << " read: ";
        for (int j = 0; j < buffer; j++)
        {
          cout << temp[j] << ", ";
        }
        cout << endl << endl;
      }
  }

  return 0;
}
