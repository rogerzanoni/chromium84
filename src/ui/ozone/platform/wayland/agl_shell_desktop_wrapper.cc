#include "ui/ozone/platform/wayland/agl_shell_desktop_wrapper.h"

#include <agl-shell-desktop-client-protocol.h>

#include "ui/ozone/platform/wayland/host/wayland_connection.h"
#include "ui/ozone/platform/wayland/host/wayland_output_manager.h"
#include "ui/ozone/platform/wayland/host/wayland_output.h"

namespace ui {

AglShellDesktop::AglShellDesktop(WaylandConnection *wconnection, std::string app_id, int port)
  : connection_(wconnection)
  , agl_shell_desktop_(wconnection->ashell_desktop())
  , app_id_(app_id)
{
    homescreen_.init(port, "wm");

    homescreen_.set_event_handler(LibHomeScreen::Event_ShowWindow, [this] (json_object* object) {
      struct json_object *obj_app_id;
      if (json_object_object_get_ex(object, "application_id", &obj_app_id)) {
        std::string app_id = json_object_get_string(obj_app_id);
        LOG(INFO) << "showWindow received for " << app_id;
        if (app_id == app_id_) {
            LOG(INFO) << "Activate app " << app_id_;

            WaylandOutputManager *wayland_manager = connection_->wayland_output_manager();
            WaylandOutput *wayland_output = wayland_manager->GetPrimaryOutput();
            agl_shell_desktop_activate_app(agl_shell_desktop_, app_id_.c_str(), nullptr, wayland_output->output());
            connection_->ScheduleFlush();
        }
      } else {
        LOG(ERROR) << "Failed to parse json object";
      }
    });
}

AglShellDesktop::~AglShellDesktop()
{
  agl_shell_desktop_destroy(agl_shell_desktop_);
}

} // namespace ui
