#include "sgproductpolicy.hpp"


std::string SGProductPolicy::hash() const
{
  std::string str("F"); // for "face"
  for (int s = 0; s < policies.size(); s++)
    {
      str += "_";
      for (auto it = policies[s].cbegin(); it != policies[s].cend(); it++)
	str += it->hash();
      str += "_";
      str += std::to_string(policies[s].size());
    }


  return str;
}
