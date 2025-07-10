#!/bin/bash

# Name executable
EXEC=./mandelbrot

# List of parameter sets: center_x center_y xside
params=(
  "-0.759 0.000 2.500"
  "-0.75 0.08 0.16"
  "-0.744567 0.121201 0.002"
  "-0.74548 0.11669 0.01276"
)

# Run the executable for each parameter set
for p in "${params[@]}"; do
  echo "Running with parameters: $p"
  $EXEC $p
done
