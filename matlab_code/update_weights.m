function weights = update_weights(input_particles, sensor, R, landmarks)
weights = ones(length(input_particles),1);
for i=1:length(landmarks)
    distance = vecnorm(input_particles - landmarks(:,i));
    pd = makedist('Normal','mu',sensor(i),'sigma',R);
    weights = pdf(pd,distance)' .* weights;
    %weights = weights + 1.e-3;
    weights = weights / sum(weights);
end
weights = weights / sum(weights);
end