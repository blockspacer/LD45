#include "DebugView.hpp"
#include "Util.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

#include <limits>
#include <numeric>

DebugView::DebugView()
{
    reset();
}

void DebugView::startFrame()
{
    m_curFrameStart = std::chrono::high_resolution_clock::now();
}

void DebugView::endFrame()
{
    constexpr std::chrono::milliseconds longFrameTime(kLongFrameTimeMS);
    auto frameLen = std::chrono::high_resolution_clock::now() - m_curFrameStart;

    if (frameLen < m_shortestFrame)
        m_shortestFrame = frameLen;
    if (frameLen > m_longestFrame)
        m_longestFrame = frameLen;
    if (frameLen >= longFrameTime)
        ++m_longFramesCounter;

    m_lastFrames[m_lastFrameIter] = frameLen;
    m_lastFrameIter = (m_lastFrameIter + 1) % m_lastFrames.size();

    m_frameCounter += 1;

    if (std::chrono::high_resolution_clock::now() - m_lastDraw > std::chrono::seconds(1))
    {
        m_lastDraw = std::chrono::high_resolution_clock::now();
        std::array<char, 2048> debugBuf;

        const int fps = m_frameCounter;
        const int ups = m_updateCounter;

        float avgFrame = std::accumulate(m_lastFrames.begin(), m_lastFrames.end(), 0, [](int total, auto b) { return total + b.count(); }) / float(m_lastFrames.size()) / 1000000;
        float avgUpdate = std::accumulate(m_lastUpdates.begin(), m_lastUpdates.end(), 0, [](int total, auto b) { return total + b.count(); }) / float(m_lastUpdates.size()) / 1000000;

        auto len = snprintf(debugBuf.data(), debugBuf.size(),
                 "=== [Debug] ===\n\nFPS: %d\nUPS: %d\n\n--- Frames: ---\nShortest: %ldms\nLongest:  %ldms\nAverage:  %.2fms\nTotal >=%dms: %ld\n\n--- Updates: ---\nShortest: %ldms\nLongest:  %ldms\nAverage:  %.2fms\nTotal >=%dms: %ld",
                 fps,
                 ups,
                 std::chrono::duration_cast<std::chrono::milliseconds>(m_shortestFrame).count(),
                 std::chrono::duration_cast<std::chrono::milliseconds>(m_longestFrame).count(),
                 avgFrame,
                 kLongFrameTimeMS,
                 m_longFramesCounter,
                 std::chrono::duration_cast<std::chrono::milliseconds>(m_shortestUpdate).count(),
                 std::chrono::duration_cast<std::chrono::milliseconds>(m_longestUpdate).count(),
                 avgUpdate,
                 kLongUpdateTimeMS,
                 m_longUpdatesCounter);

        m_debugString = std::string(debugBuf.data(), len);
        softReset();
    }
}

void DebugView::startUpdate()
{
    m_curUpdateStart = std::chrono::high_resolution_clock::now();
}

void DebugView::endUpdate()
{
    constexpr std::chrono::milliseconds longUpdateTime(kLongUpdateTimeMS);
    auto updateLen = std::chrono::high_resolution_clock::now() - m_curUpdateStart;

    if (updateLen < m_shortestUpdate)
        m_shortestUpdate = updateLen;
    if (updateLen > m_longestUpdate)
        m_longestUpdate = updateLen;

    if (updateLen >= longUpdateTime)
        ++m_longUpdatesCounter;

    m_lastUpdates[m_lastUpdateIter] = updateLen;
    m_lastUpdateIter = (m_lastUpdateIter + 1) % m_lastUpdates.size();

    m_updateCounter += 1;
}

void DebugView::reset()
{
    softReset();

    m_longFramesCounter = 0;
    m_longUpdatesCounter = 0;

    for (auto& it : m_lastFrames)
        it = std::chrono::nanoseconds(0);
    m_lastFrameIter = 0;
    for (auto& it : m_lastUpdates)
        it = std::chrono::nanoseconds(0);
    m_lastUpdateIter = 0;

    m_lastDraw = std::chrono::high_resolution_clock::now();
    m_curFrameStart = m_lastDraw;
    m_curUpdateStart = m_lastDraw;
    m_debugString.clear();
}

void DebugView::softReset()
{
    m_frameCounter = 0;
    m_updateCounter = 0;
    m_shortestFrame = std::chrono::nanoseconds(std::numeric_limits<long>::max());
    m_shortestUpdate = std::chrono::nanoseconds(std::numeric_limits<long>::max());
    m_longestFrame = std::chrono::nanoseconds(0);
    m_longestUpdate = std::chrono::nanoseconds(0);
}

void DebugView::draw(sf::RenderTarget& aTarget, sf::RenderStates states) const
{
    auto font = Util::GetDefaultFont(Font_Monospace);

    sf::Text debugString(m_debugString, font, 10u);
    debugString.setPosition(10, 10);

    aTarget.draw(debugString, states);
}
