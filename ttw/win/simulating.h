#pragma once
class Simulating
{
public:
	Simulating();
protected:
	static bool IsSimulating();
	static void SetSimulating(bool value);
private:
	static bool m_isSimulating;
};