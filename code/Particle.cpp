#include "Particle.h"

// Constructor: Initializes a particle with a specified number of points, position, and behavior
Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition) : m_A(2, numPoints)
{
    m_ttl = TTL; // Time to live for the particle
    m_numPoints = numPoints; // Number of points in the particle
    m_radiansPerSec = (float)rand() / (RAND_MAX) * MY_PI; // Random rotation speed
    m_cartesianPlane.setCenter(0, 0); // Setting the center of the cartesian plane
    m_cartesianPlane.setSize(target.getSize().x, -1.0 * target.getSize().y); // Setting the size of the cartesian plane
    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane); // Mapping click position to cartesian coordinates

    // Velocity initialization
    constexpr int MAXVEL = 500; // Maximum velocity
    constexpr int MINVEL = -100;  // Minimum velocity
    m_vx = rand() % (MAXVEL - MINVEL + 1) - MINVEL; // Random horizontal velocity
    m_vy = rand() % (MAXVEL - MINVEL + 1) - MINVEL; // Random vertical velocity
    // Randomize the direction of velocity
    if (rand() % 2) m_vx *= -1;
    if (rand() % 2) m_vy *= -1;

    // Color initialization
    m_color1 = Color::White; // Default color
    // Randomly select a second color
    switch(rand() % 6)
    {
        case 0:
            m_color2 = Color::Red;
            break;
        case 1:
            m_color2 = Color::Green;
            break;
        case 2:
            m_color2 = Color::Blue;
            break;
        case 3:
            m_color2 = Color::Yellow;
            break;
        case 4:
            m_color2 = Color::Magenta;
            break;
        case 5:
            m_color2 = Color::Cyan;
            break;
    }

    // Position initialization for each point in the particle
    double theta = static_cast<double>(rand()) / RAND_MAX * M_PI / 2;
    double dtheta = 2 * MY_PI / (numPoints - 1);
    for (int i = 0; i < numPoints; i++)
    {
        double r = rand() % (80 - 20 + 1) + 20;
        double dx = r * cos(theta);
        double dy = r * sin(theta);
        m_A(0, i) = m_centerCoordinate.x + dx;
        m_A(1, i) = m_centerCoordinate.y + dy;
        theta += dtheta;
    }
}

// Draw function: Renders the particle onto the target
void Particle::draw(RenderTarget& target, RenderStates states) const
{
    VertexArray lines(TriangleFan, m_numPoints + 1); // Creating a vertex array for the particle
    Vector2f center = static_cast<Vector2f>(target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane));
    lines[0].position = center; // Set the center point
    lines[0].color = m_color1; // Set the color of the center point
    for (int i = 1; i <= m_numPoints; i++)
    {
        // Setting the position and color for each point in the particle
        lines[i].position = static_cast<Vector2f>(target.mapCoordsToPixel({m_A(0, i - 1), m_A(1, i - 1)}, m_cartesianPlane));
        lines[i].color = m_color2;
    }
    target.draw(lines); // Drawing the particle
}

// Update function: Updates the state of the particle each frame
void Particle::update(float dt)
{
    m_ttl -= dt; // Decreasing the time to live
    rotate(dt * m_radiansPerSec); // Applying rotation
    scale(SCALE); // Applying scaling

    // Update the position based on velocity
    float dx = m_vx * dt;
    m_vy -= G * dt; // Gravity effect
    float dy = m_vy * dt;
    translate(dx, dy); // Translating the particle
}

// Translate function: Moves the particle by a certain amount
void Particle::translate(double xShift, double yShift)
{
    TranslationMatrix T(xShift, yShift, m_A.getCols());
    m_A = T + m_A; // Apply translation to all points
    m_centerCoordinate.x += xShift; // Updating the center coordinate of the particle along the x-axis by adding the x-axis shift
    m_centerCoordinate.y += yShift; // Updating the center coordinate of the particle
}

// Rotate function: Rotates the particle around its center
void Particle::rotate(double theta)
{
    Vector2f temp = m_centerCoordinate; // Storing the current center
    translate(-m_centerCoordinate.x, -m_centerCoordinate.y); // Moving particle to origin for rotation
    RotationMatrix R(theta); // Creating a rotation matrix
    m_A = R * m_A; // Applying the rotation to all points
    translate(temp.x, temp.y); // Moving particle back to its original position
}

// Scale function: Scales the particle size
void Particle::scale(double c)
{
    Vector2f temp = m_centerCoordinate; // Storing the current center
    translate(-m_centerCoordinate.x, -m_centerCoordinate.y); // Moving particle to origin for scaling
    ScalingMatrix S = c; // Creating a scaling matrix
    m_A = S * m_A; // Applying the scaling to all points
    translate(temp.x, temp.y); // Moving particle back to its original position
}

bool Particle::almostEqual(double a, double b, double eps)
{
	return fabs(a - b) < eps;
}

void Particle::unitTests()
{
    int score = 0;

    cout << "Testing RotationMatrix constructor...";
    double theta = M_PI / 4.0;
    RotationMatrix r(M_PI / 4);
    if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta))
        && almostEqual(r(0, 1), -sin(theta))
        && almostEqual(r(1, 0), sin(theta))
        && almostEqual(r(1, 1), cos(theta)))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing ScalingMatrix constructor...";
    ScalingMatrix s(1.5);
    if (s.getRows() == 2 && s.getCols() == 2
        && almostEqual(s(0, 0), 1.5)
        && almostEqual(s(0, 1), 0)
        && almostEqual(s(1, 0), 0)
        && almostEqual(s(1, 1), 1.5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing TranslationMatrix constructor...";
    TranslationMatrix t(5, -5, 3);
    if (t.getRows() == 2 && t.getCols() == 3
        && almostEqual(t(0, 0), 5)
        && almostEqual(t(1, 0), -5)
        && almostEqual(t(0, 1), 5)
        && almostEqual(t(1, 1), -5)
        && almostEqual(t(0, 2), 5)
        && almostEqual(t(1, 2), -5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    
    cout << "Testing Particles..." << endl;
    cout << "Testing Particle mapping to Cartesian origin..." << endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0)
    {
        cout << "Failed.  Expected (0,0).  Received: (" << m_centerCoordinate.x << "," << m_centerCoordinate.y << ")" << endl;
    }
    else
    {
        cout << "Passed.  +1" << endl;
        score++;
    }

    cout << "Applying one rotation of 90 degrees about the origin..." << endl;
    Matrix initialCoords = m_A;
    rotate(M_PI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j)) || !almostEqual(m_A(1, j), initialCoords(0, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            rotationPassed = false;
        }
    }
    if (rotationPassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a scale of 0.5..." << endl;
    initialCoords = m_A;
    scale(0.5);
    bool scalePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0,j)) || !almostEqual(m_A(1, j), 0.5 * initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            scalePassed = false;
        }
    }
    if (scalePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a translation of (10, 5)..." << endl;
    initialCoords = m_A;
    translate(10, 5);
    bool translatePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) || !almostEqual(m_A(1, j), 5 + initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            translatePassed = false;
        }
    }
    if (translatePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Score: " << score << " / 7" << endl;
}