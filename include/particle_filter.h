// Vector2, 2 components
typedef struct Vector2Pol {
    float r;
    float theta;
} Vector2Pol;

#define PARTICLES_NUMBERS 100
#define MAX_X 400
#define MAX_Y 400

Vector2 input_particles[PARTICLES_NUMBERS] = {0};
Vector2 output_particles[PARTICLES_NUMBERS] = {0};
Vector2 speed = {0};
Vector2Pol polSpeed = {0};
Vector2Pol polStd = {1, 1};
Vector2 filter_center = {0, 0};
Vector2 filter_center_forDisplay = {0, 0};
Vector2 last_filter_Center = {0, 0};
float weights[PARTICLES_NUMBERS];
size_t output_indexes[PARTICLES_NUMBERS] = {0};

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
        input_particles[counter].x = GetNormalRandomValue() * MAX_X + 3 * MAX_X;
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

float vecnorm(Vector2 vec) {
    return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

double normal_pdf(double x, double mu, double sigma) {
    double coefficient = 1.0 / (sigma * sqrt(2.0 * PI));
    double exponent = -((x - mu) * (x - mu)) / (2.0 * sigma * sigma);
    return coefficient * exp(exponent);
}

void update_weights(float* output_weights, 
    Vector2* input_particles, 
    float* sensor, size_t num_sensor,
    Vector2* input_landmarks, size_t num_landmarks,
    float sigma
    )
{
    // Initialize weights to 1
    for (size_t i = 0; i < PARTICLES_NUMBERS; i++) {
        weights[i] = 1.0f;
    }

    // Update weights based on landmarks
    for (size_t i = 0; i < num_landmarks; i++) {
        for (size_t j = 0; j < PARTICLES_NUMBERS; j++) {
            Vector2 diff = {input_particles[j].x - input_landmarks[i].x, input_particles[j].y - input_landmarks[i].y};
            float distance = vecnorm(diff);
            float pdf_value = normal_pdf(distance, sensor[i], sigma);
            weights[j] *= pdf_value;
        }

        // Normalize weights
        float sum_weights = 0.0f;
        for (size_t j = 0; j < PARTICLES_NUMBERS; j++) {
            sum_weights += weights[j];
        }
        for (size_t j = 0; j < PARTICLES_NUMBERS; j++) {
            weights[j] /= sum_weights;
        }
    }

    // Final normalization
    float sum_weights = 0.0f;
    for (size_t i = 0; i < PARTICLES_NUMBERS; i++) {
        sum_weights += weights[i];
    }
    for (size_t i = 0; i < PARTICLES_NUMBERS; i++) {
        weights[i] /= sum_weights;
    }

}

float uniform_random() {
    return (float)rand() / (RAND_MAX + 1.0);
}

void systematic_resample(size_t* output_indexes, float* input_weights , size_t NUM_PARTICLES)
{
    // Compute the cumulative sum of weights
    float cumulative_sum[NUM_PARTICLES];
    cumulative_sum[0] = input_weights[0];
    for (int i = 1; i < NUM_PARTICLES; i++) {
        cumulative_sum[i] = cumulative_sum[i - 1] + input_weights[i];
    }

    // Generate uniformly spaced points
    float positions[NUM_PARTICLES];
    for (int i = 0; i < NUM_PARTICLES; i++) {
        positions[i] = (uniform_random() + (float)i) / (float)NUM_PARTICLES;
    }

    // Perform the resampling
    int i = 0, j = 0;
    while (i < NUM_PARTICLES) {
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