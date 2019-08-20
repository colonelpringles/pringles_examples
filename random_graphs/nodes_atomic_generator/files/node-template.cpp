#include <random>
#include <string>

#include "message.h"
#include "parsimu.h"
#include "real.h"
#include "tuple_value.h"

#include "node{{n}}.h"

#define NODE_DEBUG false

#define PRINT_STATE this->state == INFECTED ? "INFECTED" : (this->state == HEALTHY ? "HEALTHY" : "RECOVERED")
#define PRINT_CSV_STATE(time) cout <<  "CSV," << time << "," << name << "," << (PRINT_STATE) << endl
#define PRINT_CSV_STATE_CONTAGION(time) cout <<  "CSV," << time << "," << name << "," << "INFECTING" << endl

using namespace std;

Node{{n}}::Node{{n}}(const string &name) :
Atomic(name),
name(name), {%for i in range(0,n)%} 
in{{i}}(addInputPort("in{{i}}")),
out{{i}}(addOutputPort("out{{i}}")),{%endfor%}
uniform(0.0,1.0),
hasInfectedSomeone(false)
{
{%for i in range(0,n)%}
     neighbor_to_port[{{i}}] = &out{{i}};{%endfor%}
     randomNeighbour = std::uniform_int_distribution<int>(0, {{n-1}});
     // Time dependant seed
     unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // Get exponential lambda from atomic models parameters
     double exponentialLambda = .5;
     if (ParallelMainSimulator::Instance().existsParameter(description(), "exp_lambda")) {
        exponentialLambda = str2Value(ParallelMainSimulator::Instance().getParameter(description(), "exp_lambda"));
     }
     this->exponentialLambda = exponentialLambda;

     std::cout << "exponentialLambda: " << exponentialLambda << std::endl;

    // Get exponential lambda from atomic models parameters
    // When an internal transition happens, the events that occur could be 
    // INFECTION, with P = infectedProbability
    // RECOVERY,  with P = 1 - infectedProbability
     double infectedProbability = .5;
     if (ParallelMainSimulator::Instance().existsParameter(description(), "infection_prob")) {
        infectedProbability = str2Value(ParallelMainSimulator::Instance().getParameter(description(), "infection_prob"));
     }
     this->infectedProbability = infectedProbability;

     std::cout << "infectedProbability: " << infectedProbability << std::endl;

     exponential = exponential_distribution<double>(exponentialLambda);
     gen = std::default_random_engine(seed);
}

// Initiates the n-arity node with a state of INFECTED or HEALTHY, according to a equiprobable distribution
Model &Node{{n}}::initFunction()
{
    double isInfectedProbability = uniform(gen);

    cout << description() << " got in infection lottery: " << isInfectedProbability << endl;

    if (isInfectedProbability < infectedProbability) {
        cout << description() << " is infected at first" << endl;
        this->state = INFECTED;
        holdIn(AtomicState::active, this->rand_exponential_time());
    } else {
        cout << description() << " is healthy at first" << endl;
        this->state = HEALTHY;
        passivate();
    }

    return *this;
}

Model &Node{{n}}::externalFunction(const ExternalMessage &msg)
{
    if (this->state == HEALTHY)
    {
        // It it was healthy, this is becoming infected
        this->state = INFECTED;
        holdIn(AtomicState::active, this->rand_exponential_time());
    }
    return *this;
}

Model &Node{{n}}::internalFunction(const InternalMessage &msg)
{
    switch(this->state) {
        case INFECTED:
            // This is an internal transition, which means the output function has just occurred.
            // If i'm still infected, it means I have infected someone. If not, I'm recovered.
            holdIn(AtomicState::active, this->rand_exponential_time());
            if (hasInfectedSomeone)
            {
                this->hasInfectedSomeone = false;
            }
            break;
        case RECOVERED:
            // I got recovered, passivate as is I were healthy.
            passivate();
            break;
    }

    return *this ;
}

Model &Node{{n}}::outputFunction(const CollectMessage &msg)
{
    // Im infected
    double randomUniform = uniform(gen);

    if (randomUniform < infectedProbability) {
        // Infect someone randomly, with uniform distribution
        int neighbourToInfect = randomNeighbour(gen);
        sendOutput(msg.time(), *neighbor_to_port[neighbourToInfect], this->state);    
        this->hasInfectedSomeone = true;
    } else {
        // Cure myself
        this->state = RECOVERED;
    }

    return *this ;
}

// Each infection is spread by an exponential time
VTime Node{{n}}::rand_exponential_time()
{
    double clock = exponential(gen);
    return VTime(clock);
}

