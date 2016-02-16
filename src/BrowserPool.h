/*
 * BrowserPool.h
 *
 *  Created on: 6 Feb 2016
 *      Author: bam4d
 */

#ifndef BROWSERPOOL_H_
#define BROWSERPOOL_H_

#include <list>
#include <map>
#include <mutex>

#include "include/wrapper/cef_helpers.h"

class RenderProxyHandler;
class CefBrowserHandler;

using namespace std;

class BrowserPool {
public:
	BrowserPool(int browsers);
	virtual ~BrowserPool();

	// Gets a browser instance from the pool
	// This will block until a browser is available
	int AssignBrowserSync(RenderProxyHandler* renderProxyHandler);

	// Releases the browser from the renderProxyHandler back to the pool
	void ReleaseBrowserSync(RenderProxyHandler* renderProxyHandler);

	// Start the browser session with the assigned browser
	void StartBrowserSession(RenderProxyHandler* renderProxyHandler);

	// Send a response to the client
	void SendResponse(int browser, std::string responseContent);

	// Get the render proxy handler for a specific browser
	RenderProxyHandler* GetAssignedRenderProxyHandler(int browserId);

	// Get the assigned browser for the renderProxyHandler
	int GetAssignedBrowserId(RenderProxyHandler* renderProxyHandler);

	// Number of browsers in the pool
	int Size();

	void Initialize();

private:

	std::mutex browser_routing_mutex;

	int numBrowsers_;

	// Keeps a map of handlers to their respective browsers
	std::map<int, RenderProxyHandler*> browserIdToHandler_;

	// Keeps a map of browsers to their repective handlers
	std::map<RenderProxyHandler*, int> handlerToBrowserId_;

	CefRefPtr<CefBrowserHandler> browserHandler_;
};

#endif /* BROWSERPOOL_H_ */
