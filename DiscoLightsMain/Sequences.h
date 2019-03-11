#ifndef SEQUENCEH
#define SEQUENCEH

#include "DotStrip.h"


class Sequence
{
  public:
  Sequence::Sequence(DotStrip*);
  void groovy (void);
  private:
  DotStrip *ds;
  unsigned char RED,GREEN,BLUE;
};

#endif

