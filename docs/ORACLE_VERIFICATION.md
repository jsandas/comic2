# Oracle Verification

## Overview

The oracle replay layer provides an optional, deterministic verification path for gameplay sequences. It parses simple DOSBox-style replay logs, captures runtime snapshots from the bootstrap dispatcher loop, and compares the two with configurable tolerance thresholds.

## Usage

- Build the project normally.
- Run the unit test suite to exercise the parser and snapshot comparator.
- Optional runtime flags:
  - `COMIC2_ORACLE_REPLAY=1` to enable snapshot capture during bootstrap.
  - `COMIC2_ORACLE_LOG=/path/to/replay.log` to write a replay log.

## Snapshot Format

Each snapshot contains:

- tick
- player position: x, y
- player velocity: x_vel, y_vel
- grounded/facing/hp flags
- score, gems, lives

## Comparison Algorithm

Comparison is tolerant by default:

- position tolerance: 1 pixel
- timing tolerance: 1 tick

Any mismatch beyond the threshold is reported as a textual difference entry.
