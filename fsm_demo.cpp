#include "boost/sml.hpp"
#include <iostream>

namespace sml = boost::sml;

#define SML_ENTRY_FN(name, body)            \
  struct name {                             \
    template <class E, class F, class S, class T> \
    void operator()(const E&, F&, S&, T&) const body \
  }

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
struct Stopped {};
SML_ENTRY_FN(Stopped_on_entry, { std::puts("[enter] Stopped"); });
SML_ENTRY_FN(Stopped_on_exit, { std::puts("[exit] Stopped"); });

struct Playing {};
SML_ENTRY_FN(Playing_on_entry, { std::puts("[enter] Playing"); });
SML_ENTRY_FN(Playing_on_exit, { std::puts("[exit] Playing"); });

struct Paused {};
SML_ENTRY_FN(Paused_on_entry, { std::puts("[enter] Paused"); });
SML_ENTRY_FN(Paused_on_exit, { std::puts("[exit] Paused"); });

struct Error {};
SML_ENTRY_FN(Error_on_entry, { std::puts("[enter] Error"); });
SML_ENTRY_FN(Error_on_exit, { std::puts("[exit] Error"); });

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
             state<Error>   + event<reset>          / do_reset = state<Stopped>,
             state<Stopped> + on_entry<_> / Stopped_on_entry{},
             state<Stopped> + boost::sml::on_exit<_> / Stopped_on_exit{},
             state<Playing> + on_entry<_> / Playing_on_entry{},
             state<Playing> + boost::sml::on_exit<_> / Playing_on_exit{},
             state<Paused> + on_entry<_> / Paused_on_entry{},
             state<Paused> + boost::sml::on_exit<_> / Paused_on_exit{},
             state<Error> + on_entry<_> / Error_on_entry{},
             state<Error> + boost::sml::on_exit<_> / Error_on_exit{}
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
