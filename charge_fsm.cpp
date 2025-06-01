#include "boost/sml.hpp"
#include <iostream>
#include <queue>

namespace sml = boost::sml;

struct next {};
struct render {};

struct Context {
  int counter = 0;
};

struct fsm_def {
  auto operator()() const {
    using namespace sml;

    auto counter_lt_4 = [](const next &, Context &ctx) {
      return ctx.counter < 4;
    };

    auto counter_ge_4 = [](const next &, Context &ctx) {
      return ctx.counter >= 4;
    };

    auto increment = [](const next &, Context &ctx) {
      ++ctx.counter;
      std::cout << "Charging... counter is now " << ctx.counter << "\n";
    };

    auto reset = [](const next &, Context &ctx) {
      std::cout << "Discharging and resetting counter\n";
      ctx.counter = 0;
    };

    auto transition_msg = [](const next &, Context &) {
      std::cout << "Transitioning to discharge\n";
    };

    auto render_charging = [](const render &, Context &) {
      std::cout << "Render Charging\n";
    };

    return make_transition_table(
        *state<class charging> + event<next>[counter_ge_4] / transition_msg =
            state<class discharge>,
        state<class charging> + event<next>[counter_lt_4] / increment,
        state<class discharge> + event<next> / reset = state<class charging>,
        state<class charging> + event<render> / render_charging,
        state<class discharge> +
            event<render> /
                [](sml::back::process<next, render> process_event,
                   const render &, Context &ctx) {
                  std::cout << "**Render Discharge - " << ctx.counter << " **\n";
                  process_event(render{});
                  process_event(next{});
                  std::cout << "^^ AFTER THE EMBEDDED NEXT ^^" << std::endl;
                }

    );
  }
};

int main() {
  Context ctx;
  sml::sm<fsm_def, sml::process_queue<std::queue>> sm{ctx};

  for (int i = 0; i < 10; ++i) {
    std::cout << "Normal process START" << std::endl;
    sm.process_event(render{});
    sm.process_event(next{});
    std::cout << "Normal process FINISH" << std::endl;
  }
}
