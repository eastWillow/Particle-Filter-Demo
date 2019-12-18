function particles = predict(input_particles, u, std, dt)
% u = [heading_theta,distance]
    N = length(input_particles);
    dist_predict = (u(2) * dt) + randn(N,1) * std(2);
    theta_predict = (u(1) * dt) + randn(N,1) * deg2rad(std(1));
    
    particles(1,:) = (input_particles(1,:)' + dist_predict.*cos(theta_predict))';
    particles(2,:) = (input_particles(2,:)' + dist_predict.*sin(theta_predict))';
        
end

