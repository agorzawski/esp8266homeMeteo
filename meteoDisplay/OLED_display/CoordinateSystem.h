/*
 * This is a library for scaling given  arrays (x & y) down/up to the integer axes, to be used i.e. in the led displays.
 * 2002 arek gorzawski
 */
#ifndef CoordinateSystem_h
#define CoordinateSystem_h

class CoordinateSystem
{
  public :
    CoordinateSystem(int _sx, int _sy, float xmax, float xmin, float ymax, float ymin)
    /* _sx and _sy the size of avaliable display, xmax, xmin, ymax, ymin the min/max values of valies */
    {
      sx = _sx - 10;
      sy = _sy - 10;
      a1 = sx / (xmax - xmin);
      b1 =  (-1 * sx * xmin) / (xmax - xmin);
      y1 = ymax; y2 = ymin;
      x1 = xmax; x2 = xmin;
      a2 = sy / (ymin - ymax);
      b2 = (-1 * sy * ymax) / (ymin - ymax);
    }

    ~CoordinateSystem()
    {
    }

    int getIntX(float x)
    {
      return ((int) (a1 * x) + b1);
    }

    int getIntY(float y)
    {
      return ((int) (a2 * y) + b2);
    }

    float getFloatX(int x)
    {
      return  ((x1 - y1) / (sx + 10)) * x + x1;
    }

    float getFloatY(int y)
    {
      return ((y2 - x1) / (sy + 10)) * y + y2;
    }

  private:
    float a1, a2, b1, b2, y1, y2, sx, sy, x1, x2;
};

#endif
