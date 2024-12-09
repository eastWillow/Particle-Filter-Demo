function indexes_return = systematic_resample(weights)

    N = length(weights);
    positions = ((1:N)' + rand)/N;

    indexes = ones(N,1);
    cumulative_sum = cumsum(weights);

    i = 1;
    j = 1;
    
    while (i < N) && (j < N)
        if (positions(i) < cumulative_sum(j))
            indexes(i) = j;
            i = i + 1;
        else
            j = j + 1;
        end
    end
    indexes_return = indexes;
end

