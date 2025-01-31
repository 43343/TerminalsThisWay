#include "simulating.h"


bool Simulating::m_isSimulating = false;

Simulating::Simulating() {};

bool Simulating::IsSimulating()
{
	return m_isSimulating;
}
void Simulating::SetSimulating(bool value)
{
	m_isSimulating = value;
}