#pragma once

#include <chrono>

namespace BGLRenderer
{
    class HighResolutionTimer
    {
    public:
        HighResolutionTimer()
        {
            restart();
        }

        double elapsedSeconds()
        {
            return elapsedMilliseconds() * 0.001;
        }

        double elapsedMilliseconds()
        {
            return elapsed() * 0.001;
        }

        void restart()
        {
            _startTimePoint = std::chrono::high_resolution_clock::now();
        }

        long long elapsed()
        {
            auto endTimePoint = std::chrono::high_resolution_clock::now();

            auto start = std::chrono::time_point_cast<std::chrono::microseconds>(_startTimePoint).time_since_epoch().count();
            auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

            long long duration = end - start;
            return duration;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> _startTimePoint;
    };
}
