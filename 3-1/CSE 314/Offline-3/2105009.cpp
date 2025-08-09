#include <chrono>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <random>
#include <unistd.h>
#include <vector>
#include <semaphore.h>

#define NUMBER_OF_STATIONS 4
#define MAX_ARRIVAL_TIME 15
#define NUMBER_OF_STAFF 2
#define LOGBOOK_READ_TIME 3
#define DUMMY_OPERATIVE_ID -1
#define MAX_RANDOM_READ_INTERVAL 5
#define MIN_RANDOM_READ_INTERVAL 1
#define LAMBDA_VALUE 10000.234
#define FINAL_WAIT_TIME 1

using namespace std;

//*************************** Global variables ***************************//

int N, M, x, y; // N=operatives,
                // M=unit_size,
                // x=recreation_time,
                // y=logbook_time

// Timing system
auto start_time = chrono::high_resolution_clock::now();

// Station synchronization (4 stations: TS1, TS2, TS3, TS4)
sem_t station_available[NUMBER_OF_STATIONS + 1]; // Semaphore for each station (1-indexed)

// Reader-Writer synchronization for logbook
pthread_mutex_t logbook_mutex;      // Protects logbook access
pthread_mutex_t reader_count_mutex; // Protects reader count of the logbook at any particular instance
int reader_count = 0;               // Number of active readers
int completed_operations = 0;       // Shared logbook counter

// Group synchronization
vector<sem_t> group_semaphores; // One semaphore per group
int num_groups;

// Output synchronization
pthread_mutex_t output_lock;

bool intelligence_distribution_completed = false;

//*************************** Global variables ***************************//

/**
 * function to generate a Poisson-distributed random number
 */
int get_random_number()
{
    random_device rd;
    mt19937 generator(rd());
    double lambda = LAMBDA_VALUE;
    poisson_distribution<int> poissonDist(lambda);
    return poissonDist(generator);
}

// *********************** Operative structure *********************** //
struct Operative
{
    int id;
    int assigned_station; // 1-4 (for TS1-TS4)
    int recreation_time;
    int group_id;   // Which group this operative belongs to (1-based)
    bool is_leader; // Is this operative the group leader?

    Operative(int operative_id) : id(operative_id)
    {
        assigned_station = (id % NUMBER_OF_STATIONS) + 1; // 1-based station indexing
        recreation_time = get_random_number() % x + 1;
        group_id = (id - 1) / M + 1; // 1-based group indexing
        is_leader = (id % M == 0);   // Highest ID in group is leader
    }
};

vector<Operative> operatives;
// *********************** Operative structure *********************** //

/**
 * function to get the elapsed time in milliseconds since the start of the simulation.
 */
long long get_time()
{
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(
        end_time - start_time);
    return duration.count();
}

/**
 * thread-safe output function
 */
void write_output(const string &message)
{
    pthread_mutex_lock(&output_lock);
    cout << message << endl;
    pthread_mutex_unlock(&output_lock);
}

/**
 * function to initialize synchronization primitives
 */
void initialize()
{
    // Initialize station semaphores (1-indexed)
    for (int i = 1; i <= NUMBER_OF_STATIONS; i++)
    {
        sem_init(&station_available[i], 0, 1); // Each station initially available
    }

    // initialize reader-writer synchronization
    pthread_mutex_init(&logbook_mutex, NULL);
    pthread_mutex_init(&reader_count_mutex, NULL);

    // Initialize group synchronization
    num_groups = N / M;                      // Number of complete groups
    group_semaphores.resize(num_groups + 1); // 1-indexed

    for (int i = 1; i <= num_groups; i++)
    {
        sem_init(&group_semaphores[i], 0, 0); // Start at 0, increment as operatives complete
    }

    // Initialize output mutex
    pthread_mutex_init(&output_lock, NULL);

    // Create operatives
    for (int i = 1; i <= N; i++)
    {
        operatives.emplace_back(Operative(i));
    }

    // Reset start time
    start_time = chrono::high_resolution_clock::now();
}

/**
 * Reader function for intelligence staff
 */
void *intelligence_staff_proc(void *arg)
{
    int staff_id = *(int *)arg;

    while (!intelligence_distribution_completed) // Continue until all operatives have completed their operations
    {
        // Random reading interval
        int time_between_reads = get_random_number() % MAX_RANDOM_READ_INTERVAL + MIN_RANDOM_READ_INTERVAL; // 1-6 time units
        usleep(time_between_reads * 1000);

        // Reader entry protocol
        pthread_mutex_lock(&reader_count_mutex);
        reader_count++;
        if (reader_count == 1)
        {
            pthread_mutex_lock(&logbook_mutex); // First reader locks logbook
        }
        pthread_mutex_unlock(&reader_count_mutex);

        // Reading the logbook
        write_output("Intelligence Staff " + to_string(staff_id) +
                     " began reviewing logbook at time " + to_string(get_time()) +
                     ". Operations completed = " + to_string(completed_operations));

        // Simulate reading time
        usleep(LOGBOOK_READ_TIME * 1000);

        // Reader exit
        pthread_mutex_lock(&reader_count_mutex);
        reader_count--;
        if (reader_count == 0)
        {
            pthread_mutex_unlock(&logbook_mutex); // Last reader unlocks logbook
        }
        pthread_mutex_unlock(&reader_count_mutex);
    }

    return NULL;
}

/**
 * Thread function for operative activities
 */
void *operative_proc(void *arg)
{
    Operative *op = (Operative *)arg;

    // Random arrival delay
    int arrival_time_delay = get_random_number() % MAX_ARRIVAL_TIME + 1;
    usleep(arrival_time_delay * 1000);

    // Arrive at assigned station
    write_output("Operative " + to_string(op->id) +
                 " has arrived at typewriting station " + to_string(op->assigned_station) +
                 " at time " + to_string(get_time()));

    // Wait for station to become available (no busy waiting)
    sem_wait(&station_available[op->assigned_station]);

    write_output("Operative " + to_string(op->id) +
                 " has started document recreation at typewriting station " +
                 to_string(op->assigned_station) +
                 " at time " + to_string(get_time()));

    // Simulate document recreation work
    usleep(op->recreation_time * 1000);

    write_output("Operative " + to_string(op->id) +
                 " has completed document recreation at typewriting station " +
                 to_string(op->assigned_station) +
                 " at time " + to_string(get_time()));

    // Signal that station is available (notifies ALL waiting operatives)
    sem_post(&station_available[op->assigned_station]);
    write_output("Notifying all: Typewriting station " + to_string(op->assigned_station) +
                 " is now available");

    // Signal group semaphore that this operative has completed
    sem_post(&group_semaphores[op->group_id]);

    // unit leader responsibilities
    if (op->is_leader)
    {
        // Wait until all operatives in the group have completed document recreation
        for (int i = 0; i < M; i++)
        {
            sem_wait(&group_semaphores[op->group_id]); // Wait for each group member to complete
        }

        write_output("Unit " + to_string(op->group_id) +
                     "(Group leader: " + to_string(op->id) + ")" +
                     " has completed document recreation phase at time " +
                     to_string(get_time()));

        // Notify intelligence staff about the completion of operations
        pthread_mutex_lock(&logbook_mutex);
        usleep(y * 1000);                   // Simulate logbook entry time
        write_output("Unit " + to_string(op->group_id) +
                     "(Group leader: " + to_string(op->id) + ")" +
                     " has completed intelligence distribution at time " +
                     to_string(get_time()));
        completed_operations++;               // Increment the count of completed operations
        pthread_mutex_unlock(&logbook_mutex); // Release the logbook mutex
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: ./complete <input_file> <output_file>" << endl;
        return 1;
    }

    // File handling for input and output redirection
    ifstream inputFile(argv[1]);
    streambuf *cinBuffer = cin.rdbuf(); // Save original cin buffer
    cin.rdbuf(inputFile.rdbuf());       // Redirect cin to input file

    ofstream outputFile(argv[2]);
    streambuf *coutBuffer = cout.rdbuf(); // Save original cout buffer
    cout.rdbuf(outputFile.rdbuf());       // Redirect cout to output file

    // Read input
    cin >> N >> M >> x >> y;

    // Initialize system
    initialize();

    // CREATING INTELLIGENCE STAFF THREADS
    pthread_t intelligence_staff_threads[NUMBER_OF_STAFF + 1]; // 1-indexed, so size 3
    int staff_ids[NUMBER_OF_STAFF + 1] = {0, 1, 2};            // 1-indexed staff IDs

    // creating intelligence staff threads
    for (int i = 1; i <= NUMBER_OF_STAFF; i++)
    {
        pthread_create(&intelligence_staff_threads[i], NULL, intelligence_staff_proc, &staff_ids[i]); 
    }

    // Create operative threads
    pthread_t operative_threads[N + 1]; // 1-indexed Array to hold operative threads

    operatives.reserve(N + 1);
    operatives.push_back(Operative(DUMMY_OPERATIVE_ID)); // Dummy operative at index 0

    // CREATING OPERATIVE THREADS
    for (int i = 1; i <= N; i++)
    {
        Operative newOperative = Operative(i);
        operatives.push_back(newOperative);
        pthread_create(&operative_threads[i], NULL, operative_proc, (void *)&operatives[i]); // Create thread for each operative
    }

    // Wait for all operatives to finish
    for (int i = 1; i <= N; i++)
    {
        pthread_join(operative_threads[i], NULL);
    }

    // Print summary
    write_output("All operatives have completed document recreation phase");

    // letting the intelligence staff continue reading for a bit longer
    usleep(FINAL_WAIT_TIME);

    intelligence_distribution_completed = true; // Set completed to true after all operatives have finished
    

    // Wait for intelligence staff to finish
    for (int i = 1; i <= NUMBER_OF_STAFF; i++)
    {
        pthread_join(intelligence_staff_threads[i], NULL);
    }

    // // Cleanup
    // for (int i = 1; i <= NUMBER_OF_STATIONS; i++) {
    //     sem_destroy(&station_available[i]);
    // }
    // pthread_mutex_destroy(&output_lock);
    // pthread_mutex_destroy(&logbook_mutex);
    // pthread_mutex_destroy(&reader_count_mutex);
    // for (int i = 1; i <= num_groups; i++) {
    //     sem_destroy(&group_semaphores[i]);
    // }

    // restoring cin and cout
    cin.rdbuf(cinBuffer);
    cout.rdbuf(coutBuffer);

    return 0;
}