#!/bin/bash

# Name executable
EXEC=./fractal

# List of parameter sets
params=(
  "mandelbrot -0.759 0.000 2.500"
  "mandelbrot -0.75 0.08 0.16"
  "mandelbrot -0.744567 0.121201 0.002"
  "mandelbrot -0.74548 0.11669 0.01276"
  "julia_set 0.000 0.000 0.002 -0.74543 0.11301"
  "julia_set 0.000 0.000 0.002 -0.8 0.156"
  "julia_set 0.000 0.000 0.002 -0.7269 0.1889"
  "julia_set 0.000 0.000 3 -0.4 -0.59"
)

cd build

# Run the executable for each parameter set
echo "Running with format $1"
for p in "${params[@]}"; do
  echo "Running with parameters: $p"
  $EXEC $1 $p
done
