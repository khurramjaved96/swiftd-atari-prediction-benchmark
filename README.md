# SwiftTD Atari Prediction Benchmark

SwiftTD (Swift Temporal Difference) learning algorithm benchmark on Atari games.

## Dependencies

- CMake (3.26+)
- C++17 compiler
- pkg-config
- zlib

Install on Ubuntu/Debian:
```bash
sudo apt install build-essential cmake pkg-config zlib1g-dev
```

Install on macOS:
```bash
brew install cmake pkg-config zlib
```

## Build

```bash
mkdir build
cd build
cmake ..
make -j4
```

## Usage

Check total number of configurations:
```bash
./Count --config ../configs/sample_config.json
```

Run single configuration:
```bash
./SwiftTDOnAtari --config ../configs/sample_config.json --run 0
```

Run all configurations in parallel (sample_config.json has 53 total jobs):
```bash
parallel -j <cpus> ./SwiftTDOnAtari --config ../configs/sample_config.json --run ::: {1..53}
```

