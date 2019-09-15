#include <string>
#include <random>

#include "message.h"
#include "parsimu.h"
#include "real.h"

#include "DTMC.h"

using namespace std;


DTMC::DTMC(const string &name) :

	Atomic(name),

	// Inputs

	// Outputs
	currentState_o(addOutputPort("currentState_o")),

	// internal variables
	step_time(0,0,1,0), // markov chain's time
	dist(0.0,1.0), // uniform density probability function: minval, maxval
	rng(random_device()()), // semilla aleatoria
	// rng(12345678), // semilla fija
	totalCount(0)
{

	if( ParallelMainSimulator::Instance().existsParameter( description(), "N" ) )
		N = stod( ParallelMainSimulator::Instance().getParameter( description(), "N" ) );
	else
		N = 3;

	if( ParallelMainSimulator::Instance().existsParameter( description(), "r" ) )
		r = stod( ParallelMainSimulator::Instance().getParameter( description(), "r" ) );
	else
		r = 0.5;

	if( ParallelMainSimulator::Instance().existsParameter( description(), "s" ) )
		s = stod( ParallelMainSimulator::Instance().getParameter( description(), "s" ) );
	else
		s = 0.5;

	if( ParallelMainSimulator::Instance().existsParameter( description(), "initial" ) )
		initial = stod( ParallelMainSimulator::Instance().getParameter( description(), "initial" ) );
	else
		initial = 1;

	// transition probability matrix
	P.resize(N);
	for(int i = 0 ; i < N ; i++)
	{
		//Grow Columns by n
		P[i].resize(N);
	}

	// row 0
	P[0][0] = 1 - r;
	P[0][1] = r;
	// row 1 to N-2
	for(int i = 1; i <= N-1; i++) {
		P[i][i]   = 1 - r - s;
		P[i][i-1] = s;
		P[i][i+1] = r;
	}
	// row N-1
	P[N-1][N-1] = 1 - s;
	P[N-1][N-2] = s;

	// check transition probability matrix
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			cout << P[i][j] << " ";
		}
		cout << endl;
	}

	stateCount.resize(N);
	for(int i = 0 ; i < N ; i++)
	{
		stateCount[i] = 0;
	}

	cout << "DTMC atomic succesfully created" << endl;

}


Model &DTMC::initFunction()
{

	state = this->initial;
	holdIn(AtomicState::active, this->step_time);

	// frequency
	this->stateCount[this->state-1]++,
	this->totalCount++;

  	cout << "DTMC atomic initialized" << endl;

	return *this;
}


Model &DTMC::externalFunction(const ExternalMessage &msg)
{

	// sigma = sigma - elapsed;
	return *this;
}


Model &DTMC::internalFunction(const InternalMessage &)
{

	int i; // auxiliary index
	std::vector<double> cumprob; // cumulative probability
	auto random_float = this->dist(this->rng); // flip a coin

	cumprob.resize(this->N);

	// we rely on the discrete inverse transform method to test the markov chain
	cumprob[0] = this->P[this->state-1][0];
	for (i = 1; i < this->N; i++){
		cumprob[i] = this->P[this->state-1][i] + cumprob[i-1];
	}

	i = 0;
	while(random_float >= cumprob[i]){
		i++;
	}

	this->state = i+1;

	// frequency
	this->stateCount[this->state-1]++,
	this->totalCount++;

	holdIn(AtomicState::active, this->step_time);

	return *this ;
}


Model &DTMC::outputFunction(const CollectMessage &msg)
{

	Tuple<Real> outValue{1,0,0};

	sendOutput(msg.time(), currentState_o, Tuple<Real>({this->state,this->stateCount[this->state-1],this->totalCount}));
	// cout << msg.time() << " state == " << std::to_string(this->state) << endl;

	return *this ;
}

double DTMC::get_param(const string &name)
{
    double value = 0;

    try
    {
        string param = ParallelMainSimulator::Instance().getParameter(description(), name);
        stringstream param_stream(param);

        param_stream >> value;
    }
    catch(IniRequestException &)
    {}

    return value;
}

