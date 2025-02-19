// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_SHELL_PLATFORM_LINUX_EMBEDDED_PLUGINS_TEXT_INPUT_PLUGIN_H_
#define FLUTTER_SHELL_PLATFORM_LINUX_EMBEDDED_PLUGINS_TEXT_INPUT_PLUGIN_H_

#include <rapidjson/document.h>

#include <memory>

#include <glib.h>
#include <maliit-glib/maliitbus.h>

#include "flutter/shell/platform/common/client_wrapper/include/flutter/binary_messenger.h"
#include "flutter/shell/platform/common/client_wrapper/include/flutter/method_channel.h"
#include "flutter/shell/platform/common/text_input_model.h"
#include "flutter/shell/platform/linux_embedded/window_binding_handler.h"

namespace flutter {

class TextInputPlugin {
 public:
  TextInputPlugin(BinaryMessenger* messenger, WindowBindingHandler* delegate);
  ~TextInputPlugin() = default;

  void OnKeyPressed(uint32_t keycode, uint32_t code_point);
  void DispatchEvent();

 private:
  // Sends the current state of the given model to the Flutter engine.
  void SendStateUpdate(const TextInputModel& model);

  // Sends an action triggered by the Enter key to the Flutter engine.
  void EnterPressed(TextInputModel* model);

  // Called when a method is called on |channel_|;
  void HandleMethodCall(
      const flutter::MethodCall<rapidjson::Document>& method_call,
      std::unique_ptr<flutter::MethodResult<rapidjson::Document>> result);

  void InitMaliitConnection();

  static gboolean MaliitHandleIMInitiatedHide(MaliitContext *obj,
                                              GDBusMethodInvocation *invocation,
                                              gpointer user_data);

  static gboolean MaliitHandleCommitString(MaliitContext *obj,
                                           GDBusMethodInvocation *invocation,
                                           const gchar *string,
                                           int replacement_start,
                                           int replacement_length,
                                           int cursor_pos,
                                           gpointer user_data);

  static gboolean MaliitHandleUpdatePreedit(MaliitContext *obj,
                                            GDBusMethodInvocation *invocation,
                                            const gchar *string,
                                            GVariant *formatListData,
                                            gint replaceStart,
                                            gint replaceLength,
                                            gint cursorPos,
                                            gpointer user_data);

  static gboolean MaliitHandleKeyEvent(MaliitContext *obj,
                                       GDBusMethodInvocation *invocation,
                                       gint type,
                                       gint key,
                                       gint modifiers,
                                       const gchar *text,
                                       gboolean auto_repeat,
                                       int count,
                                       guchar request_type,
                                       gpointer user_data);

  static gboolean MaliitHandleUpdateInputMethodArea(MaliitContext *obj,
                                                    GDBusMethodInvocation *invocation,
                                                    gint x,
                                                    gint y,
                                                    gint width,
                                                    gint height,
                                                    gpointer user_data);

  void MaliitShowInputMethod();
  void MaliitHideInputMethod();

  // The MethodChannel used for communication with the Flutter engine.
  std::unique_ptr<flutter::MethodChannel<rapidjson::Document>> channel_;

  // The active client id.
  int client_id_ = 0;

  // The active model. nullptr if not set.
  std::unique_ptr<TextInputModel> active_model_;

  // Keyboard type of the client. See available options:
  // https://docs.flutter.io/flutter/services/TextInputType-class.html
  std::string input_type_;

  // An action requested by the user on the input client. See available options:
  // https://docs.flutter.io/flutter/services/TextInputAction-class.html
  std::string input_action_;

  // The delegate for virtual keyboard updates.
  WindowBindingHandler* delegate_;

  GMainContext *glib_ctx_;
  GMainLoop *glib_loop_;
  MaliitServer *maliit_server_;
  MaliitContext *maliit_context_;
};

}  // namespace flutter

#endif  // FLUTTER_SHELL_PLATFORM_LINUX_EMBEDDED_PLUGINS_TEXT_INPUT_PLUGIN_H_
