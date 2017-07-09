#pragma once

//PD loop constants https://en.wikipedia.org/wiki/PID_controller
const float proportionalConst = 0.2f; 
const float derivateConst = 1.0f;

const int maxMotorSpeed = 255; //values form 0 to 255 possible
const int threshold = 400; //defines at what sensor value the surface counts as black