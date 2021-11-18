#include "InfrastOfficeTask.h"

#include "Controller.h"

const std::string asst::InfrastOfficeTask::FacilityName = "Office";

bool asst::InfrastOfficeTask::run()
{
    json::value task_start_json = json::object{
        { "task_type", "InfrastOfficeTask" },
        { "task_chain", m_task_chain }
    };
    m_callback(AsstMsg::TaskStart, task_start_json, m_callback_arg);

    set_facility(FacilityName);
    m_all_available_opers.clear();

    // 办公室只能造这一个
    set_product("HR");

    swipe_to_the_right_of_main_ui();
    enter_facility(FacilityName, 0);
    click_bottomleft_tab();

    constexpr int retry_times = 1;
    for (int i = 0; i <= retry_times; ++i) {
        if (need_exit()) {
            return false;
        }
        swipe_to_the_left_of_operlist();
        opers_detect_with_swipe();
        swipe_to_the_left_of_operlist();

        auto find_iter = std::find_if(
            m_all_available_opers.begin(), m_all_available_opers.end(),
            [&](const infrast::Oper& info) -> bool {
                return info.selected;
            });

        bool need_shift = true;
        if (find_iter != m_all_available_opers.end()) {
            switch (m_work_mode) {
            case infrast::WorkMode::Gentle:
                // 如果之前有干员在，那就不换人，直接退出
                m_all_available_opers.erase(find_iter);
                need_shift = false;
                break;
            case infrast::WorkMode::Aggressive:
                need_shift = true;
                ctrler.click(find_iter->rect);
                break;
            case infrast::WorkMode::Extreme: // TODO
                break;
            default:
                break;
            }
        }
        if (need_shift) {
            optimal_calc();
            bool ret = opers_choose();
            if (!ret) {
                m_all_available_opers.clear();
                continue;
            }
        }
        break;
    }
    click_confirm_button();
    click_return_button();

    return true;
}