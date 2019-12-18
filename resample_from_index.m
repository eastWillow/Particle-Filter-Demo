function [particles, weights] = resample_from_index(input_particles, input_weights, input_indexes)
    particles = input_particles(:,input_indexes);
    input_weights = input_weights(input_indexes);
    weights = input_weights/sum(input_weights);
end