/*
 * BrowserPool.cc
 *
 *  Created on: 6 Feb 2016
 *      Author: bam4d
 */
#include "BrowserPool.h"

#include "include/cef_browser.h"

/**
 * Local includes
 */
#include "CefBrowserHandler.h"
#include "RenderProxyHandlerImpl.h"



/**
 * The browser pool is a resource manager for CefBrowsers, responsible for allocating browsers from incoming requests
 */
BrowserPool::BrowserPool(CefRefPtr<CefBrowserHandler> cefBrowserHandler, int browsers) {
	DCHECK(browsers <= 200);

	browserHandler_ = cefBrowserHandler;

	numBrowsers_ = browsers;
	freeBrowserList_ = new MPMCQueue<int>(browsers);
}

BrowserPool::~BrowserPool() {
	delete freeBrowserList_;
}

void BrowserPool::Initialize() {

	// Start all the browsers for this browser pool
	browserHandler_->Initialize(this);
}

void BrowserPool::RegisterBrowser(CefRefPtr<CefBrowser> browser) {
	LOG(INFO)<<"registering browser";

	browserList_.push_back(browser);
	browserSession_.insert(std::make_pair(browser->GetIdentifier(),BrowserSession()));
	freeBrowserList_->blockingWrite(browser->GetIdentifier());
}

/**
 * Gets a browser instance from the pool
 * This will block until a browser is available
 */
int BrowserPool::AssignBrowserSync(RenderProxyHandler* renderProxyHandler) {
	DCHECK(renderProxyHandler != nullptr);

	// We shouldn't block other browsers trying to write to the free browser list while we are waiting for a free browser.
	// Therefore we start the mutex after we have popped the free browser


	// Get a free browser from the free browser list
	int browserId;
	if(freeBrowserList_->read(browserId)) {

		CefRefPtr<CefBrowser> freeBrowser = getBrowserById(browserId);

		{
			std::lock_guard<std::mutex> lock(browser_routing_mutex);
			// Then we assign it to the handler
			browserIdToHandler_.insert(std::make_pair(browserId, renderProxyHandler));
			handlerToBrowserId_.insert(std::make_pair(renderProxyHandler, browserId));
		}

		return browserId;
	} else {
		return -1;
	}

}

/**
 * Releases the browser from the renderProxyHandler back to the pool
 */
void BrowserPool::ReleaseBrowserSync(RenderProxyHandler* renderProxyHandler) {

	// Remove from the routing maps
	int freeBrowserId = GetAssignedBrowserId(renderProxyHandler);


	browserHandler_->ResetBrowser(getBrowserById(freeBrowserId));

	{
		std::lock_guard<std::mutex> lock(browser_routing_mutex);
		browserIdToHandler_.erase(freeBrowserId);
		handlerToBrowserId_.erase(renderProxyHandler);
	}

	// Add back to the freeBrowser list
	freeBrowserList_->write(freeBrowserId);



}

/**
 * Start the browser session with the assigned browser
 */
void BrowserPool::StartBrowserSession(RenderProxyHandler* renderProxyHandler) {
	DCHECK(renderProxyHandler != nullptr);

	// Find the browser we have assigned to this request and then start the session based on the url
	browserHandler_->StartBrowserSession(getBrowserById(GetAssignedBrowserId(renderProxyHandler)), renderProxyHandler->GetRequestedUrl());
}

/**
 * Get the render proxy handler for a specific browser
 */
RenderProxyHandler* BrowserPool::GetAssignedRenderProxyHandler(int browserId) {
	std::lock_guard<std::mutex> lock(browser_routing_mutex);
	DCHECK(browserIdToHandler_.find(browserId) != browserIdToHandler_.end());
	return browserIdToHandler_[browserId];
}

/**
 * Get the assigned browser for the renderProxyHandler
 */
int BrowserPool::GetAssignedBrowserId(RenderProxyHandler* renderProxyHandler) {
	std::lock_guard<std::mutex> lock(browser_routing_mutex);
	DCHECK(renderProxyHandler != nullptr);
	DCHECK(handlerToBrowserId_.find(renderProxyHandler) != handlerToBrowserId_.end());
	return handlerToBrowserId_[renderProxyHandler];
}

/**
 * Get the browser state by the browser Id
 */
BrowserSession* BrowserPool::GetBrowserSessionById(int browserId) {
	return &browserSession_[browserId];
}


/**
 * Get the browser based on it's unique Id
 */
CefRefPtr<CefBrowser> BrowserPool::getBrowserById(int browserId) {

	for(auto browser : browserList_) {
		if(browser->GetIdentifier() == browserId) {
			return browser;
		}
	}
	return nullptr;
}

int BrowserPool::Size() {
	return numBrowsers_;
}

