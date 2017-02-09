#include <cstdlib>

template <int N>
class CircularBuffer
{
  public:
    CircularBuffer(): _writeIndex(0), _readIndex(0), _used(0) {}
 
    void reset()
    {
      _writeIndex = _readIndex = _used = 0;
    }
 
    void write(const float* data, size_t size)
    {
      for (size_t i = 0; i < size; i++)
      {
        _buffer[(_writeIndex + i) % N] = data[i];
      }
 
      _writeIndex += size;
      _writeIndex %= N;
      _used += size;
    }
 
    void discard(size_t size)
    {
      _readIndex += size;
      _readIndex %= N;
      _used -= size;
    }
   
    void read(float* data, size_t size)
    {        
      for (size_t i = 0; i < size; i++)
      {
         data[i] = _buffer[(_readIndex + i) % N];
      }
 
      _readIndex += size;
      _readIndex %= N;
      _used -= size;
    }
 
    float read()
    {
      float r = _buffer[_readIndex];
      _readIndex += 1;
      _readIndex %= N;
      _used -= 1;
      return r;
    }
 
    const size_t& getSize() const {return N;}
    const size_t& getUsed() const {return _used;};
 
  private:
    float _buffer[N];
    size_t  _writeIndex;
    size_t  _readIndex;
    size_t  _used;
};
