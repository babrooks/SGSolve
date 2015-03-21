// Definitions for doxygen to make relation graphs
namespace std
{
  class vector<SGPoint> { public: SGPoint element; };
  class vector<vector<SGPoint> > { public: vector<SGPoint> element; };
  class vector<SGTuple> { public: SGTuple element; };
  class vector<SGAction*> { public: SGAction* element; };
  class vector<const SGAction*> { public: const SGAction* element; };
  class vector<list<SGAction> > { public: list<SGAction> element; };
  class list<SGAction> { public: SGAction element; };
  class list<SGTuple> { public: SGTuple element; };
  class list<SGIteration> { public: SGIteration element; };
  // class vector<SGTuple> { public: SGTuple element; };
}

