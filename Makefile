all : fsm_demo with_context_fsm charge_fsm

fsm_demo : fsm_demo.cpp
	g++ -std=c++14 -O2 -fno-exceptions -Wall -Wextra -Werror -pedantic -o fsm_demo fsm_demo.cpp

with_context_fsm : with_context_fsm.cpp
	g++ -std=c++14 -O2 -fno-exceptions -Wall -Wextra -Werror -pedantic -o with_context_fsm with_context_fsm.cpp

charge_fsm : charge_fsm.cpp
	g++ -std=c++17 -O2 -fno-exceptions -Wall -Wextra -Werror -pedantic -o charge_fsm charge_fsm.cpp
