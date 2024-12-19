// Vector2, 2 components
typedef struct Vector2Pol {
    float r;
    float theta;
} Vector2Pol;

#define PARTICLES_NUMBERS 100
#define MAX_X 800
#define MAX_Y 800

Vector2 input_particles[PARTICLES_NUMBERS];
Vector2 output_particles[PARTICLES_NUMBERS];
Vector2 speed = {0};
Vector2Pol polSpeed = {0};
Vector2Pol polStd = {1, 1};
Vector2 filter_center = {0, 0};
Vector2 filter_center_forDisplay = {0, 0};
Vector2 last_filter_Center = {0, 0};
float output_weights[PARTICLES_NUMBERS];
size_t output_indexes[PARTICLES_NUMBERS];

float GetNormalRandomValue() {
    float U1 = (float)GetRandomValue(1, RAND_MAX) / RAND_MAX; // 近似等價於 rand()/RAND_MAX
    float U2 = (float)GetRandomValue(1, RAND_MAX) / RAND_MAX;

    // Box-Muller
    float Z0 = sqrtf(-2.0f * logf(U1)) * cosf(2.0f * PI * U2);
    return Z0;
}

Vector2Pol CartesianToPol(Vector2 in) {
    Vector2Pol out;
    out.r=sqrt(in.x*in.x+in.y*in.y);
    out.theta=atan2(in.y,in.x);
    return out;
}

Vector2 PolToCartesian(Vector2Pol in) {
    Vector2 out;
    out.x=in.theta*cos(in.r);
    out.y=in.theta*sin(in.r);
    return out;
}

void init(Vector2* input_particles)
{
    for(size_t counter = 0; counter < PARTICLES_NUMBERS; counter++)
    {
        input_particles[counter].x = GetNormalRandomValue() * MAX_X;
        input_particles[counter].y = GetNormalRandomValue() * MAX_Y;
    }
}

//Human move is easy, but turn is not easy
void predict(Vector2* output_particles, 
Vector2* input_particles, Vector2Pol speed, Vector2Pol std, float dt)
{
    float dist_offset_predict = 0;
    float theta_offset_predict = 0;
    
    for(size_t counter = 0; counter < PARTICLES_NUMBERS; counter++)
    {
        dist_offset_predict = speed.r * dt + GetNormalRandomValue() * std.r;
        theta_offset_predict = speed.theta * dt + GetNormalRandomValue() * std.theta;

        output_particles[counter] = (Vector2)
            {input_particles[counter].x + dist_offset_predict*cosf(theta_offset_predict),
             input_particles[counter].y + dist_offset_predict*sinf(theta_offset_predict)};
    }
}

float normal_pdf(float distance, float mu, float sigma) {
    return (1.0 / (sigma * sqrt(2 * PI))) * exp(-0.5 * pow((distance - mu) / sigma, 2));
}

void update_weights(float* output_weights, 
    Vector2* input_particles, 
    float simuLeftDistance, float simuRightDistance, 
    float sigma, 
    Vector2 leftLandMarkCenter, Vector2 rightLandMarkCenter)
{
    float sum_weights = 0.0;

    for(size_t counter = 0; counter < PARTICLES_NUMBERS; counter++)
    {
        output_weights[counter] = 1.0;
    }

    for(size_t counter = 0; counter < 2; counter++)
    {
        for(size_t inner_counter = 0; inner_counter < PARTICLES_NUMBERS; inner_counter++)
        {
            switch(counter)
            {
                case 0:
                    output_weights[inner_counter] *= normal_pdf(Vector2Distance(input_particles[inner_counter], leftLandMarkCenter), simuLeftDistance, sigma);
                    break;
                case 1:
                    output_weights[inner_counter] *= normal_pdf(Vector2Distance(input_particles[inner_counter], rightLandMarkCenter), simuRightDistance, sigma);
                    break;
            }
        }
        for(size_t inner_counter = 0; inner_counter < PARTICLES_NUMBERS; inner_counter++)
        {
            sum_weights += output_weights[inner_counter];
        }
        for(size_t inner_counter = 0; inner_counter < PARTICLES_NUMBERS; inner_counter++)
        {
            output_weights[inner_counter] /= sum_weights;
        }
    }
}

void systematic_resample(size_t* output_indexes, float* input_weights)
{
    size_t positions[PARTICLES_NUMBERS];
    float cumulative_sum[PARTICLES_NUMBERS];
    double rand_offset = (float)GetRandomValue(1, RAND_MAX) / RAND_MAX;

    //Generate random positions
    for (int i = 0; i < PARTICLES_NUMBERS; i++) {
        positions[i] = (i + rand_offset) / PARTICLES_NUMBERS;
    }

    // Compute cummulative sum of weights
    cumulative_sum[0] = input_weights[0];
    for (int i = 1; i < PARTICLES_NUMBERS; i++) {
        cumulative_sum[i] = cumulative_sum[i - 1] + input_weights[i];
    }

    // Resample
    size_t i = 0;
    size_t j = 0;
    while (i < PARTICLES_NUMBERS && j < PARTICLES_NUMBERS) {
        if (positions[i] < cumulative_sum[j]) {
            output_indexes[i] = j;
            i++;
        } else {
            j++;
        }
    }
}

void resample_from_index(Vector2* output_particles, Vector2* input_particles, size_t* indexes)
{
    for(size_t counter = 0; counter < PARTICLES_NUMBERS; counter++)
    {
        output_particles[counter] = input_particles[indexes[counter]];
    }
}