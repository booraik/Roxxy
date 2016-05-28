/*
 * bufferUtils.h
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#ifndef UTILS_BUFFERUTILS_H_
#define UTILS_BUFFERUTILS_H_

#include <folly/json.h>
#include <folly/io/IOBuf.h>

using folly::dynamic;
using folly::parseJson;
using folly::toJson;

/**
 * Utility functions for folly/proxygen buffers
 */
class BufferUtils {
public:
	static std::string GetString(std::unique_ptr<folly::IOBuf> buf);
	static dynamic GetJson(std::unique_ptr<folly::IOBuf> buf);
};

#endif /* UTILS_BUFFERUTILS_H_ */
