/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#ifndef IVW_WEBBROWSERAPP_H
#define IVW_WEBBROWSERAPP_H

#include <modules/webbrowser/webbrowsermoduledefine.h>

#include <warn/push>
#include <warn/ignore/all>
#include <include/cef_app.h>
#include <include/wrapper/cef_helpers.h>
#include "include/wrapper/cef_message_router.h"
#include <warn/pop>

namespace inviwo {

/**
 * Custom interface to CefApp and CefRenderProcessHandler. Can be used to
 * set global objects within, and modify, the DOM.
 */
class WebBrowserApp : public CefApp, public CefRenderProcessHandler {
public:
    WebBrowserApp();

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

    // CefRenderProcessHandler methods:
    void OnWebKitInitialized() OVERRIDE {
        // Create the renderer-side router for query handling.
        CefMessageRouterConfig config;
        message_router_ = CefMessageRouterRendererSide::Create(config);
    }

    void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) OVERRIDE {
        // Register the JavaScripts functions with the new context.
        message_router_->OnContextCreated(browser, frame, context);
    }

    void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefV8Context> context) OVERRIDE {

        message_router_->OnContextReleased(browser, frame, context);
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) OVERRIDE {
        return message_router_->OnProcessMessageReceived(browser, source_process, message);
    }

private:
    // Handles the renderer side of query routing.
    // Adds JavaScript function "cefQuery" to the 'window' object for sending a query.
    CefRefPtr<CefMessageRouterRendererSide> message_router_;
    IMPLEMENT_REFCOUNTING(WebBrowserApp)
};

};      // namespace inviwo
#endif  // IVW_WEBBROWSERAPP_H
