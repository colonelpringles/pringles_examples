#ifndef _NODE_{{n}}_
#define _NODE_{{n}}_

#include <random>

#include "atomic.h"
#include "VTime.h"

#define STATE_ACTIVE 0
#define STATE_INACTIVE 1
#define RECOVERED 2
#define INFECTED 1
#define HEALTHY 0

/*
@ModelMetadata
name:   Node{{n}}
input_ports: {% if n > 1 %}{%for i in range(0,n-1)%}in{{i}}, {%endfor%}{% endif %}in{{n-1}}
output_ports: {% if n > 1 %}{%for i in range(0,n-1)%}out{{i}}, {%endfor%}{% endif %}out{{n-1}}
*/
class Node{{n}} : public Atomic
{
  public:
    Node{{n}}(const string &name = "Node{{n}}" );
    virtual string className() const {  return "Node{{n}}" ;}
  
  protected:
    Model &initFunction();
    Model &externalFunction( const ExternalMessage & );
    Model &internalFunction( const InternalMessage & );
    Model &outputFunction( const CollectMessage & );

  private:
    std::string name;
    {%for i in range(0,n)%} 
    const Port &in{{i}};
    Port &out{{i}};{%endfor%}

    std::map<int, Port*> neighbor_to_port;
    int state;

    std::default_random_engine gen;
    std::exponential_distribution<> exponential;
    std::uniform_real_distribution<double> uniform;
    std::uniform_int_distribution<int> randomNeighbour;

    bool hasInfectedSomeone;

    float infectedProbability;

    VTime rand_exponential_time();
};

#endif
