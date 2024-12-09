clear
global particles_numbers max_x max_y trajectory
global previous_x previous_y zs dt landmarks
global NL sensor_std_err particles weights

particles_numbers = 100;
max_x = 800;
max_y = 800;
tag_x = max_x/2.0;
tag_y = max_y/2.0;
trajectory = [[0;0]];

previous_x = -1.0;
previous_y = -1.0;
zs = [];
dt = 0.01;
%landmarks = [[144;73], [410;13], [336;175], [718;159], [178;484], [665;464]];
landmarks = [[144;73], [410;13]];
NL = length(landmarks);
sensor_std_err = 5;

particles = [(max_x)*rand(1,particles_numbers);(max_y)*rand(1,particles_numbers)];
weights = ones(length(particles),1);