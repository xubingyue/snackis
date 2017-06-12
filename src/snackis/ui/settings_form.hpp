#ifndef SNACKIS_UI_SETTINGS_FORM_HPP
#define SNACKIS_UI_SETTINGS_FORM_HPP

#include "snackis/ui/view_form.hpp"
#include "snackis/ui/text_field.hpp"

namespace snackis {
namespace ui {
  struct SettingsForm: public ViewForm {
    TextField name, email,
      load_folder, save_folder,
      imap_url, imap_port, imap_user, imap_pass, imap_poll,
      smtp_url, smtp_port, smtp_user, smtp_pass, smtp_poll;
    
    SettingsForm(View &view, Footer &ftr);
  };

  bool run(SettingsForm &frm);
}}

#endif
