#include <imgui.h>

#include <Engine/Utils/MonitoringDebugWindow.hpp>



MonitoringDebugWindow::MonitoringDebugWindow(const glm::vec2& pos, const glm::vec2& size) :
    DebugWindow("Monitoring", pos, size)
{
    _msgList.push_back(FMT_MSG("Collision system : %f secondes", 0.0012564));
}

MonitoringDebugWindow::~MonitoringDebugWindow() {}

void    MonitoringDebugWindow::build()
{
    if (!ImGui::Begin(_title.c_str(), &_displayed, ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }
    // Set Window params
    ImGui::SetWindowSize(ImVec2(_size.x, _size.y), ImGuiSetCond_Always);
    ImGui::SetWindowPos(ImVec2(_pos.x, _pos.y), ImGuiSetCond_Always);

    // Draw components
    /*ImGui::TextUnformatted(_logger->getLog().begin(), _logger->getLog().end());

    if (ImGui::GetItemRectSize().x > _size.x)
        _size.x = ImGui::GetItemRectSize().x;

    // A Log was added since last update
    if ((uint32_t)_logger->getLog().size() > _lastLogSize)
    {
        _lastLogSize = _logger->getLog().size();
        ImGui::SetScrollHere(1.0f);
    }*/

    for (auto&& msg : _msgList)
    {
        ImGui::Text(msg.c_str());
        ImGui::Separator();
    }

    ImGui::End();
}

void    MonitoringDebugWindow::registerMsg(std::string msg)
{
    _msgList.push_back(msg);
}