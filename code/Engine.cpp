#include "Engine.h"

Engine::Engine()
{
    m_Window.create(VideoMode::getDesktopMode(), "Particles");
}

void Engine::run()
{
    srand(time(0));
    Clock clock;
    cout << "Starting Particle unit tests..." << endl;
    Particle p(m_Window, 4, { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    cout << "Unit tests complete.  Starting engine..." << endl;

    while (m_Window.isOpen())
    {
        clock.restart();
        input();
        update(clock.getElapsedTime().asSeconds());
        draw();
    }
}

void Engine::input()
{
    Event event;
    while(m_Window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            m_Window.close();
        }
        if (event.type == Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == Mouse::Left)
            {
                constexpr int NUM_PARTICLES = 5;
                constexpr int RANDLOW = 25;
                constexpr int RANDHIGH = 50;
                int numPoints;
                for (int i = 0; i < NUM_PARTICLES; i++)
                {
                    numPoints = (rand() % (RANDHIGH - RANDLOW + 1)) + RANDLOW;
                    Particle particle(m_Window, numPoints, {event.mouseButton.x, event.mouseButton.y});
                    m_particles.push_back(particle);
                }
            }
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Escape))
    {
        m_Window.close();
    }
}

void Engine::update(float dtAsSeconds)
{
    for (vector<Particle>::iterator i = m_particles.begin(); i != m_particles.end();)
    {
        if (i->getTTL() > 0.0)
        {
            i->update(dtAsSeconds);
            i++;
        }
        else
        {
            i = m_particles.erase(i);
        }
    }
}

void Engine::draw()
{
    m_Window.clear();
    for (auto& p : m_particles)
    {
        m_Window.draw(p);
    }
    m_Window.display();
}