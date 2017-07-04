/**
 * 2017 bartosz bielawski, arek gorzawski
 */
#pragma once
#include <cstdlib>
// #include <iostream>
// using namespace std;

template <typename T, int N>
class CircularBuffer
{
  public:
    CircularBuffer(): _writeIndex(0), _readIndex(-1), _used(0) {}

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

      _readIndex += size;
      _readIndex %= N;

      _used += size;
      if (_used > N) _used = N;
    }

    void discard(size_t size)
    {
      _readIndex -= size;
      _readIndex %= N;
      _writeIndex -= size;
      _used -= size;
    }

    void read(T* data, size_t size)
    {
      size_t  _readIndexTemp = _readIndex;
      _readIndex -= size;
      for (size_t i = 0; i < size; i++)
      {
         data[i] = _buffer[(_readIndex + i) % N];
      }
      _readIndex = _readIndexTemp;
    }

    T read()
    {
      T r = _buffer[_readIndex];
      return r;
    }

    const size_t& getSize() const {return N;}
    const size_t& getUsed() const {return _used;};

  private:
    T _buffer[N];
    size_t  _writeIndex;
    size_t  _readIndex;
    size_t  _used;
};
