#include <include/common.h>

SIMState sim_state = { .state = SIM_STOP };

int is_exit_status_bad() {
  int good = (sim_state.state == SIM_END && sim_state.halt_ret == 0) ||
    (sim_state.state == SIM_QUIT);
  return !good;
}
