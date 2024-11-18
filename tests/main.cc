#include "unit.hh"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	return unit_state::instance().run_tests();
}