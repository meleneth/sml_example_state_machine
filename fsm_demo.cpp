#include "boost/sml.hpp"
#include <iostream>

namespace sml = boost::sml;

// Events
struct play {};
struct pause {};
struct stop {};
struct error_occurred {};
struct reset {};

// Guards
auto is_ok = [] { std::puts("[guard] system OK"); return true; };
auto is_error = [] { std::puts("[guard] system ERROR"); return false; };

// Actions
auto do_play = [] { std::puts("[action] Playing"); };
auto do_pause = [] { std::puts("[action] Pausing"); };
auto do_stop = [] { std::puts("[action] Stopping"); };
auto do_reset = [] { std::puts("[action] Resetting"); };

// States with entry/exit actions
struct Stopped {
    auto on_entry() const { std::puts("[enter] Stopped"); }
    auto on_exit() const  { std::puts("[exit] Stopped"); }
};

struct Playing {
    auto on_entry() const { std::puts("[enter] Playing"); }
    auto on_exit() const  { std::puts("[exit] Playing"); }
};

struct Paused {
    auto on_entry() const { std::puts("[enter] Paused"); }
    auto on_exit() const  { std::puts("[exit] Paused"); }
};

struct Error {
    auto on_entry() const { std::puts("[enter] Error"); }
    auto on_exit() const  { std::puts("[exit] Error"); }
};

// Transition table
struct player {
    auto operator()() const {
        using namespace sml;
        return make_transition_table(
            *state<Stopped> + event<play>  [is_ok]   / do_play  = state<Playing>,
             state<Playing> + event<pause>          / do_pause = state<Paused>,
             state<Paused>  + event<play>           / do_play  = state<Playing>,
             state<Playing> + event<stop>           / do_stop  = state<Stopped>,
             state<Playing> + event<error_occurred> / []{ std::puts("[error] Transitioning to Error"); } = state<Error>,
             state<Error>   + event<reset>          / do_reset = state<Stopped>
        );
    }
};

int main() {
    sml::sm<player> sm;

    sm.process_event(play{});            // Should start playing
    sm.process_event(pause{});           // Pauses
    sm.process_event(play{});            // Resumes playing
    sm.process_event(error_occurred{});  // Goes to error state
    sm.process_event(reset{});           // Recovers to stopped
}
