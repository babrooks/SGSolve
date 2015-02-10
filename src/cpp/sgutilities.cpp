#include "sgexception.hpp"
#include "sgutilities.hpp"

int indexToVector(int index, 
		  vector<int> & v, 
		  const vector<int> & sizes)
{
  if (index < 0)
    throw(SGException(SGException::OUT_OF_BOUNDS));

  if(sizes.size()==0)
    throw(SGException(SGException::EMPTY_TUPLE));

  v.resize(sizes.size(),0);

  for (int coordinate = 0;
       coordinate < sizes.size();
       coordinate++)
    {
      v[coordinate] = index % sizes[coordinate];
      index -= v[coordinate]; index /= sizes[coordinate];
    }
  return index;
}

int vectorToIndex(const vector<int> & v, 
		  const vector<int> & sizes)
{
  int index = 0;

  if (v.size() != sizes.size())
    throw(SGException(SGException::TUPLE_SIZE_MISMATCH));
  for (int coordinate = sizes.size()-1;
       coordinate >= 0;
       coordinate -- )
    {
      index *= sizes[coordinate];
      index += v[coordinate];
    }
  return index;
}
