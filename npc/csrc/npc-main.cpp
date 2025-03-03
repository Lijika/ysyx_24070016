#include "include/common.h"
#include "include/sim.h"

int main(int argc, char** argv) {
  sim_init();

  init_monitor(argc, argv);

  sdb_mainloop(); //sdb run

  sim_finish();
  // assert(0);
  return is_exit_status_bad();
}