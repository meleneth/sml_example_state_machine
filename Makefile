all : fsm_demo with_context_fsm

fsm_demo : fsm_demo.cpp
	g++ -std=c++14 -O2 -fno-exceptions -Wall -Wextra -Werror -pedantic -o fsm_demo fsm_demo.cpp

with_context_fsm : with_context_fsm.cpp
	g++ -std=c++14 -O2 -fno-exceptions -Wall -Wextra -Werror -pedantic -o with_context_fsm with_context_fsm.cpp
