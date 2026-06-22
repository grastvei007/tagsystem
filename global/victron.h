#ifndef VICTRON_H
#define VICTRON_H

namespace global::victron
{

// State of operation
enum class CS{
	Off = 0,
	LowPower = 1,
	Fault = 2,
	Bulk = 3,
	Absorption = 4,
	Float = 5,
	Storage = 6,
	Equalize = 7,
	Inverting = 9,
	PowerSupply = 11,
	StartingUp = 245,
	RepeatedAbsorption = 246,
	AutoEqualize = 247,
	BatterySafe = 248,
	ExternalControl = 252
};

} // end namespace


#endif // VICTRON_H
