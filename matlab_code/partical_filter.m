%% simu tag walking
% Reference http://ros-developer.com/2019/04/10/parcticle-filter-explained-with-python-code-from-scratch/
for counter = 1:100

    tag_y = tag_y + randn()*10;
    tag_x = tag_x + randn()*10;
    center = [tag_x;tag_y];
    last_center = [previous_x;previous_y];
    plot(center(1),center(2),'o','MarkerSize',30);
    hold on
    plot(landmarks(1,:),landmarks(2,:),'^','MarkerSize',30);

    %trajectory = [trajectory center];
    if previous_x >0
        [heading_theta,distance]=cart2pol(norm(center(1)-last_center(1)),norm(center(2)-last_center(2)));
        std = [30,10]; %[heading_theta(deg),distance]

        particles = predict(particles, [heading_theta,distance], std, dt);

        sensor = vecnorm(landmarks - center) + randn(1,NL) * sensor_std_err;

        weights = update_weights(particles, sensor, 50, landmarks);

        indexes = systematic_resample(weights);

        [particles, weights] = resample_from_index(particles, weights, indexes);
    end
        previous_x = tag_x;
        previous_y = tag_y;

    scatter(particles(1,:),particles(2,:),[],normalize(weights,'range'),'filled');
    colorbar
    colormap jet
    axis image
    xlim([0 max_x])
    ylim([0 max_y])
    grid on
    hold off

    pause(.01)

end