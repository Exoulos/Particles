#include "Engine.h"

// Constructor: Initializes the main window for the particle simulation
Engine::Engine()
{
    // Create the main window with desktop video mode settings and "Particles" title
    m_Window.create(VideoMode::getDesktopMode(), "Particles");
}

// Run function: Main loop of the engine
void Engine::run()
{
    srand(time(0)); // Seed the random number generator for the number of points in the created particle
    Clock clock; // Clock to measure elapsed time
    cout << "Starting Particle unit tests..." << endl;
    // Create a test particle and run unit tests
    Particle p(m_Window, 4, { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    cout << "Unit tests complete.  Starting engine..." << endl;

    // Main loop: runs as long as the window is open
    while (m_Window.isOpen())
    {
        Time elapsed = clock.restart(); // Restart the clock for each loop
        float elapsedSeconds = elapsed.asSeconds();
        input(); // Process input events
        update(elapsedSeconds); // Update particle states based on elapsed time
        draw(); // Draw particles on the window
    }
}

// Input function: Handles user inputs and events
void Engine::input()
{
    Event event;
    while(m_Window.pollEvent(event)) // Poll for pending events
    {
        if (event.type == Event::Closed)
        {
            m_Window.close(); // Close the window if close event is triggered
        }
        if (event.type == Event::MouseButtonPressed)
        {
            // Create particles on left mouse button press
            if (event.mouseButton.button == Mouse::Left)
            {
                constexpr int NUM_PARTICLES = 5; // Number of particles to create
                constexpr int RANDLOW = 25; // Lower bound for random number of points
                constexpr int RANDHIGH = 50; // Upper bound for random number of points
                int numPoints;
                for (int i = 0; i < NUM_PARTICLES; i++)
                {
                    numPoints = (rand() % (RANDHIGH - RANDLOW + 1)) + RANDLOW;
                    // Create a new particle at the mouse position with a random number of points
                    Particle particle(m_Window, numPoints, {event.mouseButton.x, event.mouseButton.y});
                    m_particles.push_back(particle); // Add the particle to the particle list
                }
            }
        }
    }
    // Close the window if Escape key is pressed
    if (Keyboard::isKeyPressed(Keyboard::Escape))
    {
        m_Window.close();
    }
}

// Update function: Updates the state of all particles
void Engine::update(float dtAsSeconds)
{
    // Iterate through the particle list and update each particle
    for (vector<Particle>::iterator i = m_particles.begin(); i != m_particles.end();)
    {
        if (i->getTTL() > 0.0) // Check if the particle's time-to-live is still positive
        {
            i->update(dtAsSeconds); // Update the particle
            i++; // Move to next particle
        }
        else
        {
            i = m_particles.erase(i); // Remove the particle if its time-to-live is zero
        }
    }
}

// Draw function: Draws all particles on the window
void Engine::draw()
{
    m_Window.clear(); // Clear the window before drawing
    for (auto& p : m_particles) // Iterate through all particles
    {
        m_Window.draw(p); // Draw each particle
    }
    m_Window.display(); // Display the drawn content
}
