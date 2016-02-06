/*
 * RenderProxyHandlerFactory.cpp
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "RenderProxyHandlerFactory.h"

#include "include/base/cef_logging.h"
#include "RenderProxyHandler.h"


RenderProxyHandlerFactory::RenderProxyHandlerFactory(int port) {
	port_ = port;
	LOG(INFO) << "ProxygenServer server starting....";

	// Do something smart here like pool browsers.
	//Don't want to start and stop browser instances all the time, this is probably inefficient
	//browserHandler_ = new CefBrowserHandler();
}


RenderProxyHandlerFactory::~RenderProxyHandlerFactory() {
	LOG(INFO) << "destroying RenderProxyHandlerFactory";
	//delete browserHandler_;
}

void RenderProxyHandlerFactory::onServerStart(folly::EventBase* evb) noexcept {
	LOG(INFO) << "Cef server started, listening for connections on port: " << port_;
}

void RenderProxyHandlerFactory::onServerStop() noexcept {
	LOG(INFO) << "Cef server stopped, no longer listening or connections";
}

RequestHandler* RenderProxyHandlerFactory::onRequest(RequestHandler* handler, HTTPMessage* message) noexcept {
	LOG(INFO) << "request received, need to do something with this request now";

	// The proxy handler and the browser handler are killed automatically, so we don't need to keep references here
	return new RenderProxyHandler(CefRefPtr<CefBrowserHandler>(new CefBrowserHandler()));
}


