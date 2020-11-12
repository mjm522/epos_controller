#!/usr/bin/env python

import epos_boost
import time

ep = epos_boost.EPOS4Boost()
ep.open_device()
ep.set_rls_econder()
ep.get_rls_econder()
ep.min_position()
ep.set_target_position_profile(-10000)
time.sleep(4)
ep.set_target_velocity_profile(500)
ep.close_device()

