#include "boost/sml.hpp"
#include <iostream>

namespace sml = boost::sml;

// Our context (stateful data)
struct PlayerContext {
    bool has_license = false;
    void play()  { std::puts("[ctx] play()"); }
    void pause() { std::puts("[ctx] pause()"); }
};

// Events
struct play {};
struct pause {};

// States
struct Stopped {};
struct Playing {};

// FSM definition
struct player_fsm {
    auto operator()() const {
        using namespace sml;

        // Guards and actions capture the injected context
        auto can_play = [](const PlayerContext& ctx) {
            std::cout << "[guard] has_license? " << ctx.has_license << '\n';
            return ctx.has_license;
        };

        auto start_playback = [](PlayerContext& ctx) {
            ctx.play();
        };

        auto pause_playback = [](PlayerContext& ctx) {
            ctx.pause();
        };

        return make_transition_table(
            *state<Stopped> + event<play>  [can_play]    / start_playback = state<Playing>,
             state<Playing> + event<pause>               / pause_playback = state<Stopped>
        );
    }
};

int main() {
    PlayerContext ctx;
    ctx.has_license = true; // try flipping this to false to block the transition

    sml::sm<player_fsm> sm{ctx};  // inject context into FSM

    sm.process_event(play{});    // should call ctx.play()
    sm.process_event(pause{});   // should call ctx.pause()
}
