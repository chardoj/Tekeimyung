#pragma once

#include <memory>
#include <vector>
#include <map>

#include <Engine/Utils/Debug.hpp>
#include <Engine/Utils/Utils.hh>
#include <Engine/Utils/Timer.hpp>
#include <Engine/Utils/DebugWindow.hpp>

#define ENABLE_COLOR    true

#define FMT_MSG(format, ...)  (formatMessage(format, ## __VA_ARGS__))

typedef struct sMonitoring
{
    std::string         name;
    float               timeSec;
    float               avgTimeSec;
    float               oldAvg;
    std::vector<float>  timeLogs;
}                       tMonitoring;

class MonitoringDebugWindow : public DebugWindow
{
public:
    MonitoringDebugWindow(const glm::vec2& pos = glm::vec2(0, 650), const glm::vec2& size = glm::vec2(350, 300));
    virtual ~MonitoringDebugWindow();

    static std::shared_ptr<MonitoringDebugWindow>   getInstance();

    virtual void                                    build();

    uint16_t                                        registerSystem(std::string name);
    void                                            updateSystem(uint16_t key, tMonitoring newData);

private:
    float                                           calcTimeAverage(std::vector<float> timeLogs);

private:
    static std::shared_ptr<MonitoringDebugWindow>   _monitoringDebugWindow;

    std::map<uint16_t, tMonitoring>                 _systemsRegistered;
    Timer                                           _checkSec;
};

