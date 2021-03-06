#pragma once

#include <SFML/Graphics/Drawable.hpp>

#include <array>
#include <string>
#include <chrono>

namespace sf { class RenderTarget; }

class Application;

class DebugView : public sf::Drawable
{
public:
    enum
    {
        kFramesToCalc = 50,
        kLongFrameTimeMS = 20,
        kLongUpdateTimeMS = 5
    };

    DebugView();

    void init(Application& aApplication);

    void startFrame();
    void endFrame();

    void startUpdate();
    void endUpdate();

    void reset();

    void draw(sf::RenderTarget& aTarget, sf::RenderStates states) const;

private:
    void softReset();

    Application* m_application;

    size_t m_frameCounter,
           m_updateCounter,
           m_longFramesCounter,
           m_longUpdatesCounter;

    std::chrono::nanoseconds m_shortestFrame,
          m_shortestUpdate,
          m_longestFrame,
          m_longestUpdate;

    std::array<std::chrono::nanoseconds, kFramesToCalc> m_lastFrames,
        m_lastUpdates;

    size_t m_lastFrameIter,
           m_lastUpdateIter;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastDraw,
        m_curFrameStart,
        m_curUpdateStart;

    std::array<char, 2048> m_debugString;
};
