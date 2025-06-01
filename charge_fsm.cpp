#include "boost/sml.hpp"
#include <iostream>

namespace sml = boost::sml;

struct next {};  // Event
struct render {};  // Event

// External context
struct Context {
    int counter = 0;
};

// States
struct charging {};
struct discharge {};

struct fsm_def {
    auto operator()() const {
        using namespace sml;

        auto counter_lt_4 = [](const next&, Context& ctx) {
            return ctx.counter < 4;
        };

        auto counter_ge_4 = [](const next&, Context& ctx) {
            return ctx.counter >= 4;
        };

        auto increment = [](const next&, Context& ctx) {
            ++ctx.counter;
            std::cout << "Charging... counter is now " << ctx.counter << "\n";
        };

        auto reset = [](const next&, Context& ctx) {
            std::cout << "Discharging and resetting counter\n";
            ctx.counter = 0;
        };

        auto transition_msg = [](const next&, Context&) {
            std::cout << "Transitioning to discharge\n";
        };

        auto render_charging = [](const render&, Context&){
          std::cout << "Render Charging" << std::endl;
        };

        auto render_discharge = [](const render&, Context&){
          std::cout << "Render Discharge" << std::endl;
        };

        return make_transition_table(
            *state<charging> + event<next> [ counter_ge_4 ] / transition_msg = state<discharge>,
             state<charging> + event<next> [ counter_lt_4 ] / increment      ,
             state<discharge> + event<next> / reset                          = state<charging>,
             state<charging> + event<render> / render_charging,
             state<discharge> + event<render> / render_discharge
        );
    }
};

int main() {
    Context ctx;
    sml::sm<fsm_def> sm{ctx};

    for (int i = 0; i < 10; ++i) {
        sm.process_event(render{});
        sm.process_event(next{});
    }
}
