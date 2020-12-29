#include <random>
#include "rng.h"

namespace {
    pcg_extras::seed_seq_from<std::random_device> seed_source;
}

namespace malgtk {
    pcg64 rng {seed_source};
}
