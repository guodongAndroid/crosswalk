// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_RUNTIME_RENDERER_XWALK_CONTENT_RENDERER_CLIENT_H_
#define XWALK_RUNTIME_RENDERER_XWALK_CONTENT_RENDERER_CLIENT_H_

#include <memory>
#include <string>
#include <vector>

#include "base/compiler_specific.h"
#include "base/files/file.h"
#include "base/strings/string16.h"
#include "content/public/renderer/content_renderer_client.h"
#include "ui/base/page_transition_types.h"
#include "xwalk/extensions/renderer/xwalk_extension_renderer_controller.h"
#if defined(OS_ANDROID)
#include "xwalk/runtime/renderer/android/xwalk_render_thread_observer.h"
#else
#include "xwalk/runtime/renderer/xwalk_render_thread_observer_generic.h"
#endif

namespace visitedlink {
class VisitedLinkSlave;
}

namespace xwalk {

class XWalkRenderThreadObserver;

// When implementing a derived class, make sure to update
// `in_process_browser_test.cc` and `xwalk_main_delegate.cc`.
class XWalkContentRendererClient :
    public content::ContentRendererClient,
    public extensions::XWalkExtensionRendererController::Delegate {
 public:
  static XWalkContentRendererClient* Get();

  XWalkContentRendererClient();
  ~XWalkContentRendererClient() override;

  // ContentRendererClient implementation.
  void RenderThreadStarted() override;
  void RenderFrameCreated(content::RenderFrame* render_frame) override;
  void RenderViewCreated(content::RenderView* render_view) override;
  bool IsExternalPepperPlugin(const std::string& module_name) override;
  unsigned long long VisitedLinkHash(const char* canonical_url, size_t length) override;
  bool IsLinkVisited(unsigned long long link_hash) override;

  bool OverrideCreatePlugin(content::RenderFrame* render_frame, const blink::WebPluginParams& params,
                            blink::WebPlugin** plugin) override;

  bool ShouldFork(blink::WebLocalFrame* frame, const GURL& url, const std::string& http_method,
                  bool is_initial_navigation, bool is_server_redirect, bool* send_referrer) override;
  bool WillSendRequest(blink::WebLocalFrame* frame, ui::PageTransition transition_type, const blink::WebURL& url,
                       std::vector<std::unique_ptr<content::URLLoaderThrottle>>* throttles,
                       GURL* new_url) override;
  // Notifies that a document element has been inserted in the frame's document.
  // This may be called multiple times for the same document. This method may
  // invalidate the frame.
  void RunScriptsAtDocumentStart(content::RenderFrame* render_frame) override;

  // Notifies that the DOM is ready in the frame's document.
  // This method may invalidate the frame.
  void RunScriptsAtDocumentEnd(content::RenderFrame* render_frame) override;

  // Notifies that the window.onload event is about to fire.
  // This method may invalidate the frame.
  void RunScriptsAtDocumentIdle(content::RenderFrame* render_frame) override;

  void DidInitializeServiceWorkerContextOnWorkerThread(v8::Local<v8::Context> context,
                                                       int64_t service_worker_version_id,
                                                       const GURL& service_worker_scope, const GURL& script_url)
                                                           override;
  void WillDestroyServiceWorkerContextOnWorkerThread(v8::Local<v8::Context> context, int64_t service_worker_version_id,
                                                     const GURL& service_worker_scope, const GURL& script_url) override;

  void AddSupportedKeySystems(std::vector<std::unique_ptr<::media::KeySystemProperties>>* key_systems) override;

  bool ShouldReportDetailedMessageForSource(const base::string16& source) const override;

  // Returns true if the embedder has an error page to show for the given http
  // status code. If so |error_domain| should be set to according to WebURLError
  // and the embedder's GetNavigationErrorHtml will be called afterwards to get
  // the error html.
  bool HasErrorPage(int http_status_code) override;

#if defined(OS_ANDROID)
  bool HandleNavigation(content::RenderFrame* render_frame, bool is_content_initiated,
                        bool render_view_was_created_by_renderer, blink::WebFrame* frame,
                        const blink::WebURLRequest& request, blink::WebNavigationType type,
                        blink::WebNavigationPolicy default_policy, bool is_redirect) override;
#endif

 protected:
  std::unique_ptr<XWalkRenderThreadObserver> xwalk_render_thread_observer_;

 private:
  // XWalkExtensionRendererController::Delegate implementation.
  void DidCreateModuleSystem(extensions::XWalkModuleSystem* module_system) override;

  std::unique_ptr<extensions::XWalkExtensionRendererController> extension_controller_;

  std::unique_ptr<visitedlink::VisitedLinkSlave> visited_link_slave_;

  void GetNavigationErrorStrings(content::RenderFrame* render_frame, const blink::WebURLRequest& failed_request,
                                 const blink::WebURLError& error, std::string* error_html,
                                 base::string16* error_description) override;
  void GetNavigationErrorStringsForHttpStatusError(content::RenderFrame* render_frame,
                                                   const blink::WebURLRequest& failed_request,
                                                   const GURL& unreachable_url, int http_status,
                                                   std::string* error_html, base::string16* error_description) override;

  DISALLOW_COPY_AND_ASSIGN(XWalkContentRendererClient)
  ;
};

}  // namespace xwalk

#endif  // XWALK_RUNTIME_RENDERER_XWALK_CONTENT_RENDERER_CLIENT_H_
